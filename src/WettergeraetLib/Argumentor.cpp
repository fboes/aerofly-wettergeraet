#include "stdafx.h"
#include "Argumentor.h"

const char* Argumentor::APP_VERSION = "1.2.0";
#if _WIN64
const char* Argumentor::APP_TARGET = "64bit";
#else
const char* Argumentor::APP_TARGET = "32bit";
#endif

char * Argumentor::getEnv(const char * varName)
{
	char *pValue;
	size_t len;
	errno_t err = _dupenv_s(&pValue, &len, varName);
	return pValue;
	// 	free(pValue);
}

std::string Argumentor::showHelp(std::string cmd)
{
	return "Usage: " + cmd + " [options...] [FILE]\n"
		+ "Copy METAR information from URL into your Aerofly FS2 configuration file.\n"
		+ "If no options are supplied, the required information will be asked for.\n"
		+ "Arguments:\n"
		+ "    [FILE]             File to modify. Defaults to 'main.mcf' in standard\n"
		+ "                       Aerofly document path.\n"
		+ "Options:\n"
		+ "    --url <URL>        Fetch response via HTTP from <URL>.\n"
		+ "                       If URL contains 'XXXX' this will be replaced by <ICAO>.\n"
		+ "                       Defaults to URL of AvWX.\n"
		+ "    --icao <ICAO>      ICAO code of airport the METAR will be fetched for.\n"
		+ "                       If this is set to '?' the value will be asked for.\n"
		+ "                       If this contains 'DEP', ICAO airport code will be fetched\n"
		+ "                       from Aerofly FS 2 flightplan departure airport.\n"
		+ "                       If this contains 'ARR', ICAO airport code will be fetched\n"
		+ "                       from Aerofly FS 2 flightplan arrival airport.\n"
		+ "    --apikey <APIKEY>  Sent HTTP header 'X-API-Key' set to <APIKEY>.\n"
		+ "    --response <TYPE>  How to interpret HTTP response. 'json' is default.\n"
		+ "                       Set this to 'raw' if the response is plain text.\n"
		+ "                       Set this to 'json' if the response is JSON object.\n"
		+ "    --metar <METAR>    Supply a valid METAR code enclosed in '\"'.\n"
		+ "                       This will disable HTTP fetching.\n"
		+ "                       If this is set to '?' the value will be asked for.\n"
		+ "    --hours <HOURS>    Offset time by <HOURS> hours, e.g. '-8'\n"
		+ "    --dry-run          Do not save 'main.mcf'\n"
		+ "    --quiet            No console output\n"
		+ "    --verbose          Show debug output\n"
		+ "    --version          Show version information and exit\n"
		+ "    --help             Display this help and exit\n";
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
			if (currentArg == "--url") {
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
