#include "pch.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include "BoShed.h"
#include "FetchUrl.h"
#include "MetarParser.h"
#include "AeroflyWeather.h"
#include "AeroflyBlender.h"
#include "AeroflyConfigFile.h"

using namespace std;

// Show help output for CLI parameters
static void showHelp(std::string cmd)
{
	std::cerr << "Usage: " << cmd << " [options...] [FILE]\n"
		<< "Copy METAR information from URL into your Aerofly FS2 configuration file.\n"
		<< "If no options are supplied, the required information will be asked for.\n"
		<< "Arguments:\n"
		<< "    [FILE]             File to modify. Defaults to 'main.mcf' in standard\n"
		<< "                       Aerofly document path.\n"
		<< "Options:\n"
		<< "    --url <URL>        Fetch response via HTTP from <URL>.\n"
		<< "                       If URL contains 'XXXX' this will be replaced by <ICAO>.\n"
		<< "                       Defaults to URL of AvWX.\n"
		<< "    --icao <ICAO>      ICAO code of airport the METAR will be fetched for.\n"
		<< "    --apikey <APIKEY>  Sent HTTP header 'X-API-Key' set to <APIKEY>.\n"
		<< "    --response <TYPE>  How to interpret HTTP response. 'json' is default.\n"
		<< "                       Set this to 'text' if the response is plain text.\n"
		<< "                       Set this to 'json' if the response is JSON object.\n"
		<< "    --metar <METAR>    Supply a valid METAR code enclosed in '\"'.\n"
		<< "                       This will disable HTTP fetching.\n"
		<< "    --dry-run          Do not save 'main.mcf'\n"
		<< "    --quiet            No console output\n"
		<< "    --verbose          Show debug output\n"
		<< "    --help             Display this help and exit\n"
		<< std::endl;
}

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
		std::cout << BoShed::progress(aerofly.cloudHeight[i], "Height") << endl;
		std::cout << BoShed::progress(aerofly.cloudDensity[i], "Density") << endl;
	}
}

// Show METAR data via STDOUT
void showMetar(MetarParser metar) {
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
		"Visibility   %f statute miles, %f meters\n",
		metar.visibility.statuteMiles, metar.visibility.meters
	);
	MetarCloud * cloud = &metar.clouds[0];
	printf(
		"Cloud 0      Min %d, max %d at %dft, %dm\n",
		cloud->densityMinimum, cloud->densityMaximum, cloud->baseFeetAgl, cloud->baseMetersAgl
	);
	cloud = &metar.clouds[1];
	printf(
		"Cloud 1      Min %d, max %d at %dft, %dm\n",
		cloud->densityMinimum, cloud->densityMaximum, cloud->baseFeetAgl, cloud->baseMetersAgl
	);
	cloud = &metar.clouds[2];
	printf(
		"Cloud 2      Min %d, max %d at %dft, %dm\n",
		cloud->densityMinimum, cloud->densityMaximum, cloud->baseFeetAgl, cloud->baseMetersAgl
	);
	if (metar.ceiling) {
		cloud = metar.ceiling;
		printf(
			"Ceiling      Min %d, max %d at %dft, %dm\n",
			cloud->densityMinimum, cloud->densityMaximum, cloud->baseFeetAgl, cloud->baseMetersAgl
		);
	}
	printf(
		"Temperature  %f deg C, %f deg F, Dewpoint  %f deg C, %f deg F, Humidity %f%%\n",
		metar.temperature.degreesCelsius, metar.temperature.degreesFahrenheit, metar.temperature.dewpointCelsius, metar.temperature.dewpointFahrenheit, metar.temperature.humidity
	);
	printf(
		"Pressure     %fkpa, %fInch Hg\n",
		metar.barometer.kpa, metar.barometer.inhg
	);
	std::cout << "Flight category  " << metar.getFlightCategory() << endl;
}



// ----------------------------------------------------------------------------

// Main programme
int main(int argc, char* argv[])
{
	string icaoCode = "";
	string url = "http://avwx.rest/api/metar/XXXX?options=&format=json&onfail=cache";
	unsigned short response = FetchUrl::MODE_JSON;
	string apikey = "";
	string metarString = "";
	bool isDryRun = false;

	// 0: quiet; 1: normal; 2: verbose
	unsigned short verbosity = 1;
	string filename = "%USERPROFILE%\\Documents\\Aerofly FS 2\\main.mcf";

#ifdef _DEBUG
	icaoCode = "KSFO";
	//url = "https://3960.org/metar/XXXX.txt";
	//response = FetchUrl::MODE_RAW;
	//metarString = "KSFO 281756Z 04004KT 9SM BKN037 OVC047 12/05 A3017 RMK AO2 SLP214 T01170050 10117 20094 53012";
	metarString = "METAR KTTN 051853Z 04011KT 1/2SM VCTS SN FZFG BKN003 OVC010 M02/M02 A3006 RMK AO2 TSB40 SLP176 P0002 T10171017=";
	isDryRun = true;
	verbosity = 2;
#endif

	// Parsing CLI parameters
	// @see http://www.cplusplus.com/articles/DEN36Up4/

	string currentArg = "";
	for (int i = 1; i < argc; ++i) {
		currentArg = string(argv[i]);

		if (currentArg == "--help") {
			showHelp(argv[0]);
			exit(EXIT_FAILURE);
		}
		else if (currentArg == "--url") {
			url = (i + 1 < argc) ? string(argv[++i]) : url;
		}
		else if (currentArg == "--icao") {
			icaoCode = (i + 1 < argc) ? string(argv[++i]) : icaoCode;
		}
		else if (currentArg == "--response") {
			response = (i + 1 < argc && string(argv[++i]) == "text") ? FetchUrl::MODE_RAW : FetchUrl::MODE_JSON;
		}
		else if (currentArg == "--apikey") {
			apikey = (i + 1 < argc) ? string(argv[++i]) : apikey;
		}
		else if (currentArg == "--metar") {
			metarString = (i + 1 < argc) ? string(argv[++i]) : metarString;
		}
		else if (currentArg == "--dry-run") {
			isDryRun = true;
		}
		else if (currentArg == "--quiet") {
			verbosity = 0;
		}
		else if (currentArg == "--verbose") {
			verbosity = 2;
		}
		else {
			filename = argv[i];
		}
	}

	// Fetch remote data via HTTP(S)

	if (metarString == "" && url != "") {
		if (icaoCode == "") {
			cout << "Please enter an ICAO code: ";
			cin >> icaoCode;
		}

		FetchUrl urlFetcher;
		if (verbosity > 1) {
			std::cout << "URL          " << url << endl;
		}
		try {
			metarString = urlFetcher.fetch(url, icaoCode, response);
		}
		catch (std::invalid_argument& e) {
			std::cerr << "\x1B[31m" << e.what() << "\033[0m" << endl;
			exit(EXIT_FAILURE);
		}
	}
	if (verbosity > 0) {
		std::cout << "METAR        " << metarString << endl;
	}

	if (metarString == "") {
		std::cerr << "\x1B[31mNo METAR code found\033[0m" << endl;
		exit(EXIT_FAILURE);
	}

	// Parse METAR data

	MetarParser metar;
	try {
		metar.convert(metarString);
	}
	catch (std::invalid_argument& e) {
		std::cerr << "\x1B[31m" << e.what() << "\033[0m" << endl;
		exit(EXIT_FAILURE);
	}
	if (verbosity > 1) {
		showMetar(metar);
	}

	// Convert METAR data into Aerofly data

	AeroflyWeather aerofly;
	aerofly.setFromMetar(metar);
	if (verbosity > 0) {
		showAerofly(aerofly);
	}

	// Open Aerofly configuration file

	AeroflyConfigFile mainConfig(filename);
	mainConfig.load();
	mainConfig.setFromAeroflyObject(aerofly);

	if (!isDryRun) {
		if (verbosity > 0) {
			std::cout << "\nSaving file " << mainConfig.getFilename() << endl;
		}
		mainConfig.save();
	}

	return 0;
}
