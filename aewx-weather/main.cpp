#include "pch.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include "Argumentor.h"
#include "BoShed.h"
#include "FetchUrl.h"
#include "MetarParserSimple.h"
#include "AeroflyWeather.h"
#include "AeroflyBlender.h"
#include "AeroflyConfigFile.h"

using namespace std;

// Output AeroflyWeather to STDOUT
void showAerofly(AeroflyWeather aerofly) {
	std::cout << "\nTime" << endl;
	printf(" Date  UTC   %d-%d-%d\n", aerofly.year, aerofly.month, aerofly.day);
	std::cout << BoShed::progress(aerofly.hours / 24.0, "Hours UTC", 24, "h") << endl;

	std::cout << "\nWind" << endl;
	std::cout << BoShed::progress(aerofly.windDirection / 360.0, "Direction", 360, "deg") << endl;
	std::cout << BoShed::progress(aerofly.windStrength, "Strength") << endl;
	std::cout << BoShed::progress(aerofly.windTurbulence, "Turbulence") << endl;
	std::cout << BoShed::progress(aerofly.thermalActivity, "Thermal activity") << endl;
	std::cout << BoShed::progress(aerofly.visibility, "Visibility") << endl;

	for (int i = 0; i < 3; ++i) {
		std::cout << "\nCloud layer" + std::to_string(i + 1) << endl;
		std::cout << BoShed::progress(aerofly.clouds[i].height, "Height") << endl;
		std::cout << BoShed::progress(aerofly.clouds[i].density, "Density") << endl;
	}
}

// Show METAR data via STDOUT
void showMetar(MetarParserSimple metar) {
	std::cout << "ICAO code    " << metar.icao << endl;
	printf(
		"Date         %d-%d-%d %d:%dZ\n",
		metar.observed.year, metar.observed.month, metar.observed.day, metar.observed.hours, metar.observed.minutes
	);
	printf(
		"Wind         %d (VAR %d) degrees - %fkts (Gust %fkts)\n",
		metar.wind.degrees, metar.wind.degreesFrom, metar.wind.speedKts, metar.wind.gustKts
	);
	printf(
		"Visibility   %f meters\n",
		metar.visibilityMeters
	);
	for (int i = 0; i < 3; ++i) {
		printf(
			"Cloud 0      Min %d, max %d at %dft\n",
			metar.clouds[i].densityMinimum, metar.clouds[i].densityMaximum, metar.clouds[i].baseFeetAgl
		);
	}
	if (metar.ceiling) {
		printf(
			"Ceiling      Min %d, max %d at %dft\n",
			metar.ceiling->densityMinimum, metar.ceiling->densityMaximum, metar.ceiling->baseFeetAgl
		);
	}
	printf(
		"Temperature  %f deg C, Dewpoint  %f deg C, Humidity %f%%\n",
		metar.temperatureCelsius, metar.dewpointCelsius, metar.getHumidity()
	);
	printf(
		"Pressure     %fkpa\n",
		metar.barometerKpa
	);
	std::cout << "Flight category  " << metar.getFlightCategory() << endl;
}

// Return origin / desitnation ICAO code from flightplan depending on inIcao bein "DEP" or "ARR".
std::string getIcaoFromFlightplan(std::string inIcao, std::tuple<std::string, std::string> flightplan) {
	std::string origin;
	std::string destination;
	std::tie(origin, destination) = flightplan;
	inIcao = (inIcao == "DEP") ? origin : destination;
	return inIcao;
}

// Show error, exit with failure.
void dieWithError(std::invalid_argument e) {
	std::cerr << "\x1B[31m" << e.what() << "\033[0m" << endl;
	exit(EXIT_FAILURE);
}

// ----------------------------------------------------------------------------

// Main programme
int main(int argc, char* argv[])
{
	Argumentor argumentor(argc, argv);

#ifdef _DEBUG
	argumentor.icaoCode = "KSFO";
	//argumentor.url = "https://3960.org/metar/XXXX.txt";
	//response = FetchUrl::MODE_RAW;
	//argumentor.metarString = "KSFO 281756Z 04004KT 9SM BKN037 OVC047 12/05 A3017 RMK AO2 SLP214 T01170050 10117 20094 53012";
	//argumentor.metarString = "KDVO 022335Z AUTO 4SM BR BKN007 BKN013 12/12 A2988 RMK AO2";
	argumentor.metarString = "METAR KTTN 051853Z 04011KT 1/2SM VCTS SN FZFG BKN003 OVC010 M02/M02 A3006 RMK AO2 TSB40 SLP176 P0002 T10171017=";
	argumentor.isDryRun = true;
	argumentor.verbosity = 2;
#endif

	// Open Aerofly configuration file

	AeroflyConfigFile mainConfig(argumentor.filename);
	try {
		mainConfig.load();
	}
	catch (std::invalid_argument& e) {
		dieWithError(e);
	}

	// Fetch remote data via HTTP(S)

	if (argumentor.metarString == "?") {
		cout << "Please enter a METAR string: ";
		getline(cin, argumentor.metarString);
	}
	if (argumentor.metarString == "" && argumentor.url != "") {
		if (argumentor.icaoCode == "" || argumentor.icaoCode == "?") {
			cout << "Please enter an ICAO code: ";
			getline(cin, argumentor.icaoCode);
		}
		if (argumentor.icaoCode == "DEP" || argumentor.icaoCode == "ARR" || argumentor.icaoCode == "") {
			argumentor.icaoCode = getIcaoFromFlightplan(argumentor.icaoCode, mainConfig.getFlightplan());
		}

		FetchUrl urlFetcher;
		if (argumentor.verbosity > 1) {
			std::cout << "URL          " << argumentor.url << endl;
		}
		try {
			argumentor.metarString = urlFetcher.fetch(argumentor.url, argumentor.icaoCode, argumentor.response, argumentor.apikey);
		}
		catch (std::invalid_argument& e) {
			dieWithError(e);
		}
	}
	if (argumentor.verbosity > 0) {
		std::cout << "METAR        " << argumentor.metarString << endl;
	}

	if (argumentor.metarString == "") {
		dieWithError(std::invalid_argument("No METAR code found"));
	}

	// Parse METAR data

	MetarParserSimple metar;
	try {
		metar.convert(argumentor.metarString);
	}
	catch (std::invalid_argument& e) {
		dieWithError(e);
	}
	if (argumentor.verbosity > 1) {
		showMetar(metar);
	}

	// Convert METAR data into Aerofly data

	AeroflyWeather aerofly;
	aerofly.setFromMetar(metar);
	if (argumentor.verbosity > 0) {
		showAerofly(aerofly);
	}

	// Modify Aerofly Config file

	mainConfig.setFromAeroflyObject(aerofly);

	if (!argumentor.isDryRun) {
		if (argumentor.verbosity > 0) {
			std::cout << "\nSaving file " << mainConfig.getFilename() << endl;
		}
		try {
			mainConfig.save();
		}
		catch (std::invalid_argument& e) {
			dieWithError(e);
		}
	}

	return 0;
}
