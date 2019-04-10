#include "stdafx.h"
#include "Argumentor.h"
#include <regex>

const char* Argumentor::APP_VERSION = "1.2.1";
#if _WIN64
const char* Argumentor::APP_TARGET = "64-bit";
#else
const char* Argumentor::APP_TARGET = "32-bit";
#endif

char * Argumentor::getEnv(const char * varName)
{
	char *pValue;
	size_t len;
	errno_t err = _dupenv_s(&pValue, &len, varName);
	return pValue;
	// 	free(pValue);
}

std::string Argumentor::showHelpOption(std::string label, std::string description, const int indentLength, const int columnLength)
{

	label = label.substr(0, columnLength - indentLength - 2);
	std::string spacer = std::string(columnLength, ' ');

	std::string string = std::string(indentLength, ' ');
	string += label + std::string(columnLength - indentLength - label.length(), ' ');
	string += std::regex_replace(
		description,
		std::regex("\n"),
		"$&" + spacer
	);
	string += "\n";
	return string;
}

std::string Argumentor::showHelp(std::string cmd)
{
	return "Usage: " + cmd + " [options...] [FILE]\n"
		+ "Copy METAR information from URL into your Aerofly FS2 configuration file.\n"
		+ "If no options are supplied, the required information will be asked for.\n"
		+ "Arguments:\n"
		+ this->showHelpOption("[FILE]", "Absolute file location of your `main.mcf`. Defaults to\nyour standard Aerofly document path.")
		+ "Options:\n"
		+ this->showHelpOption("--file <FILE>", "Like `[FILE]`, see above.")
		+ this->showHelpOption("--url <URL>", "Fetch response via HTTP from <URL>.\nIf URL contains `XXXX` this will be replaced by <ICAO>.\nDefaults to URL of AvWX.")
		+ this->showHelpOption("--icao <ICAO>", "ICAO code of airport the METAR will be fetched for.\nIf this is set to `?` the value will be asked for.\nIf this contains `DEP`, ICAO airport code will be fetched\nfrom Aerofly FS 2 flightplan departure airport.\nIf this contains `ARR`, ICAO airport code will be fetched\nfrom Aerofly FS 2 flightplan arrival airport.")
		+ this->showHelpOption("--apikey <APIKEY>", "Sent HTTP header `X-API-Key` set to <APIKEY> for\nall HTTP API calls.")
		+ this->showHelpOption("--response <TYPE>", "How to interpret HTTP response. `json` is default.\nSet this to `raw` if the response is plain text.\nSet this to `json` if the response is JSON object.")
		+ this->showHelpOption("--metar <METAR>", "Supply a valid METAR code enclosed in `\"`.\nThis will disable HTTP fetching.\nIf this is set to `?` the value will be asked for.")
		+ this->showHelpOption("--hours <HOURS>", "Offset time read from METAR code by <HOURS> hours, e.g. '-8'.")
		;
}

std::string Argumentor::showVersion(std::string appname)
{
	return ((appname != "") ? appname + " " : "")
		+ std::string(Argumentor::APP_VERSION)
		+ " (" + std::string(Argumentor::APP_TARGET) + ")"
		;
}

Argumentor::Argumentor()
{
	// Getting ENV variable values
	auto envUrl = this->getEnv("AEROFLYWX_URL");
	if (envUrl && envUrl != "") {
		strcpy_s(this->url, 512, envUrl);
	}
	auto envApikey = this->getEnv("AEROFLYWX_APIKEY");
	if (envApikey && envApikey != "") {
		strcpy_s(this->apikey, 64, envApikey);
	}
	auto envResponse = this->getEnv("AEROFLYWX_RESPONSE");
	if (envResponse && envResponse != "") {
		this->response = (envResponse == "raw") ? FetchUrl::MODE_RAW : FetchUrl::MODE_JSON;
	}
	auto envHours = this->getEnv("AEROFLYWX_HOURS");
	if (envHours && envHours != "") {
		this->hours = std::stoi(envHours);
	}
	auto envFilename = this->getEnv("AEROFLYWX_FILE");
	if (envFilename && envFilename != "") {
		strcpy_s(this->filename, 512, envFilename);
	}
}

Argumentor::~Argumentor()
{
}

void Argumentor::getArgs(int argc, char * argv[])
{

	// Reading CLI arguments
	std::string currentArg = "";
	for (int i = 1; i < argc; ++i) {
		currentArg = std::string(argv[i]);

		if (currentArg == "--help") {
			this->isShowHelp = true;
		}
		else if (currentArg == "--version") {
			this->isShowVersion = true;
		}
		else if (currentArg == "--dry-run") {
			this->isDryRun = true;
		}
		else if (currentArg == "--quiet") {
			this->verbosity = 0;
		}
		else if (currentArg == "--verbose") {
			this->verbosity = 2;
		}
		else if (i + 1 < argc) {
			if (currentArg == "--file") {
				strcpy_s(this->filename, 512, argv[i]);
			}
			else if (currentArg == "--url") {
				strcpy_s(this->url, 512, argv[++i]);
			}
			else if (currentArg == "--icao") {
				strcpy_s(this->icaoCode, 8, argv[++i]);
			}
			else if (currentArg == "--response") {
				this->response = (strcmp(argv[++i], "raw") == 0) ? FetchUrl::MODE_RAW : FetchUrl::MODE_JSON;
			}
			else if (currentArg == "--apikey") {
				strcpy_s(this->apikey, 64, argv[++i]);
			}
			else if (currentArg == "--metar") {
				strcpy_s(this->metarString, 512, argv[++i]);
			}
			else if (currentArg == "--hours") {
				this->hours = std::stoi(argv[++i]);
			}
		}
		else {
			strcpy_s(this->filename, 512, argv[i]);
		}
	}
}
