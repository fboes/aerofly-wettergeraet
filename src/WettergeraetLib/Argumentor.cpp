#include "stdafx.h"
#include "Argumentor.h"
#include <sstream>
#include <iterator>

const char* Argumentor::APP_VERSION = "1.5.0";
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
	auto labelWidth = columnLength - indentLength - 1;
	auto columnWidth = columnLength - indentLength - (int)label.length();

	label = label.substr(0, labelWidth);
	std::string spacer = "\n" + std::string(columnLength, ' ');

	std::string string = std::string(indentLength, ' ');
	string += label + std::string(columnWidth, ' ');
	string += this->wordwrap(description, 72 - columnLength, spacer);
	string += "\n";
	return string;
}

std::string Argumentor::wordwrap(std::string candidate, const int maxLineLength, std::string wrapper)
{
	std::string output = "";

	std::string currentWord = "";
	std::stringstream candidateWords(candidate);
	int currentLineLength = 0;
	int currentWordLength = 0;

	while (candidateWords >> currentWord) {
		currentWordLength = (int)currentWord.length();
		if (currentLineLength + currentWordLength < maxLineLength) {
			if (output != "") {
				output += " ";
				currentWordLength++;
			}
			currentLineLength += currentWordLength;
		}
		else {
			output += wrapper;
			currentLineLength = 0;
		}
		output += currentWord;
	}

	return output;
}

std::string Argumentor::showHelp(std::string cmd)
{
	return "Usage: " + cmd + " [options...] [FILE]\n"
		+ "Copy METAR information from URL into your Aerofly FS2 configuration file.\n"
		+ "If no options are supplied, the required information will be asked for.\n"
		+ "Arguments:\n"
		+ this->showHelpOption("[FILE]", "Absolute file location of your `main.mcf`. Defaults to your standard Aerofly document path.")
		+ "Options:\n"
		+ this->showHelpOption("--file <FILE>", "Like `[FILE]`, see above.")
		+ this->showHelpOption("--url <URL>", "Fetch response via HTTP from <URL>. If URL contains `XXXX` this will be replaced by <ICAO>. Defaults to URL of AvWX.")
		+ this->showHelpOption("--icao <ICAO>", "ICAO code of airport the METAR will be fetched for. If this is set to `?` the value will be asked for. If this contains `DEP`, ICAO airport code will be fetched from Aerofly FS 2 flightplan departure airport. If this contains `ARR`, ICAO airport code will be fetched from Aerofly FS 2 flightplan arrival airport.")
		+ this->showHelpOption("--apikey <APIKEY>", "Sent HTTP header `X-API-Key` set to <APIKEY> for all HTTP API calls.")
		+ this->showHelpOption("--response <TYPE>", "How to interpret HTTP response. `json` is default. Set this to `raw` if the response is plain text. Set this to `json` if the response is JSON object.")
		+ this->showHelpOption("--metar <METAR>", "Supply a valid METAR code enclosed in `\"`. This will disable HTTP fetching. If this is set to `?` the value will be asked for.")
		+ this->showHelpOption("--metarfile <FILE>", "Read METAR information from <FILE>. This will disable HTTP fetching. If this is set to `?` the value will be asked for.")
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
				strcpy_s(this->filename, 512, argv[++i]);
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
			else if (currentArg == "--metarfile") {
				strcpy_s(this->metarfile, 512, argv[++i]);
			}
			else if (currentArg == "--hours") {
				this->hours = std::stoi(argv[++i]);
			}
		}
		else {
			if (strstr(argv[i], ".rwx") != NULL || strstr(argv[i], ".txt") != NULL) {
				strcpy_s(this->metarfile, 512, argv[i]);
			}
			else {
				strcpy_s(this->filename, 512, argv[i]);
			}
		}
	}
}
