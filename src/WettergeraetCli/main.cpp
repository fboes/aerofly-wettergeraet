#include "pch.h"
#include <iostream>
#include <sstream>
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
			"Cloud %d      Min %d, max %d at %dft\n",
			i, metar.clouds[i].densityMinimum, metar.clouds[i].densityMaximum, metar.clouds[i].baseFeetAgl
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

// Return origin / destination ICAO airport code from flightplan depending on inIcao bein "DEP" or "ARR".
std::string getIcaoFromFlightplan(std::string inIcao, std::tuple<std::string, std::string> flightplan) {
	std::string origin;
	std::string destination;
	std::tie(origin, destination) = flightplan;
	inIcao = (inIcao == "DEP") ? origin : destination;
	return inIcao;
}

// Read plain file and return contents
std::string getFile(std::string filename) {
	std::fstream fileStream;
	fileStream.open(filename, std::ios::in | std::ios::binary);
	if (!fileStream.is_open())
	{
		throw std::invalid_argument("Could no open " + filename + " for reading");
	}
	std::ostringstream contents;
	contents << fileStream.rdbuf();
	fileStream.close();
	return contents.str();
}

// Show error, exit with failure.
int dieWithError(std::invalid_argument e) {
	std::cerr << "\x1B[31m" << e.what() << "\033[0m" << endl;
	return EXIT_FAILURE;
}

// ----------------------------------------------------------------------------

// Main programme
int main(int argc, char* argv[])
{
	Argumentor argumentor;
	argumentor.getArgs(argc, argv);

	if (argumentor.isShowHelp) {
		std::cout << argumentor.showHelp(argv[0]) << endl;
		exit(EXIT_FAILURE);
	}
	else if (argumentor.isShowVersion) {
		std::cout << argumentor.showVersion() << endl;
		exit(EXIT_FAILURE);
	}

#ifdef _DEBUG
	strcpy_s(argumentor.icaoCode, 8, "KSFO");
	//strcpy_s(argumentor.url, "https://3960.org/metar/XXXX.txt");
	//response = FetchUrl::MODE_RAW;
	//strcpy_s(argumentor.metarString, 512, "KSFO 281756Z 04004KT 9SM BKN037 OVC047 12/05 A3017 RMK AO2 SLP214 T01170050 10117 20094 53012");
	//strcpy_s(argumentor.metarString, 512, "KDVO 022335Z AUTO 4SM BR BKN007 BKN013 12/12 A2988 RMK AO2");
	strcpy_s(argumentor.metarString, 512, "METAR KTTN 051853Z 04011KT 1/2SM VCTS SN FZFG BKN003 OVC010 M02/M02 A3006 RMK AO2 TSB40 SLP176 P0002 T10171017=");
	argumentor.hours = -20;
	argumentor.isDryRun = true;
	argumentor.verbosity = 2;
#endif

	// Open Aerofly configuration file

	AeroflyConfigFile mainConfig(argumentor.filename);
	try {
		mainConfig.load();
	}
	catch (std::invalid_argument& e) {
		return dieWithError(e);
	}

	// Parse METAR data

	if (strcmp(argumentor.metarfile, "?") == 0) {
		cout << "Please enter a METAR filename: ";
		cin.getline(argumentor.metarfile, 512);
	}
	if (strlen(argumentor.metarfile) != 0) {
		try {
			strcpy_s(argumentor.metarString, 512, getFile(argumentor.metarfile).c_str());
		}
		catch (std::invalid_argument & e) {
			return dieWithError(e);
		}
	}
	if (strcmp(argumentor.metarString, "?") == 0) {
		cout << "Please enter a METAR string: ";
		cin.getline(argumentor.metarString, 512);
	}
	if (strlen(argumentor.metarString) == 0 && strlen(argumentor.url) != 0) {
		if (strlen(argumentor.icaoCode) == 0 || strcmp(argumentor.icaoCode, "?") == 0) {
			cout << "Please enter an ICAO code: ";
			cin.getline(argumentor.icaoCode, 8);
			//getline(cin, argumentor.icaoCode);
		}
		if (strcmp(argumentor.icaoCode, "DEP") == 0 || strcmp(argumentor.icaoCode, "ARR") == 0 || strlen(argumentor.icaoCode) == 0) {
			strcpy_s(argumentor.icaoCode, 8, getIcaoFromFlightplan(argumentor.icaoCode, mainConfig.getFlightplan()).c_str());
		}

		FetchUrl urlFetcher;
		if (argumentor.verbosity > 1) {
			std::cout << "URL          " << argumentor.url << endl;
		}
		try {
			strcpy_s(argumentor.metarString, 512, urlFetcher.fetch(argumentor.url, argumentor.icaoCode, argumentor.response, argumentor.apikey).c_str());
		}
		catch (std::invalid_argument& e) {
			return dieWithError(e);
		}
	}
	if (argumentor.verbosity > 0) {
		std::cout << "METAR        " << argumentor.metarString << endl;
	}

	if (strlen(argumentor.metarString) == 0) {
		return dieWithError(std::invalid_argument("No METAR code found"));
	}

	// Parse METAR data

	MetarParserSimple metar;
	try {
		metar.convert(argumentor.metarString);
		metar.addHours(argumentor.hours);
	}
	catch (std::invalid_argument& e) {
		return dieWithError(e);
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
			return dieWithError(e);
		}
	}

	return EXIT_SUCCESS;
}
