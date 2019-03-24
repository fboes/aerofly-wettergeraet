#include "stdafx.h"
#include "Argumentor.h"

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
		+ "    --help             Display this help and exit\n";
}

Argumentor::Argumentor()
{
	// Getting ENV variable values
	if (getenv("AEROFLYWX_URL") && getenv("AEROFLYWX_URL") != "") {
		strcpy(this->url, getenv("AEROFLYWX_URL"));
	}
	if (getenv("AEROFLYWX_APIKEY") && getenv("AEROFLYWX_APIKEY") != "") {
		strcpy(this->apikey, getenv("AEROFLYWX_APIKEY"));
	}
	if (getenv("AEROFLYWX_RESPONSE") && getenv("AEROFLYWX_RESPONSE") != "") {
		this->response = ( getenv("AEROFLYWX_RESPONSE") == "raw") ? FetchUrl::MODE_RAW : FetchUrl::MODE_JSON;
	}
	if (getenv("AEROFLYWX_HOURS") && getenv("AEROFLYWX_HOURS") != "") {
		this->hours = std::stoi(getenv("AEROFLYWX_HOURS"));
	}
	if (getenv("AEROFLYWX_FILE") && getenv("AEROFLYWX_FILE") != "") {
		strcpy(this->filename, getenv("AEROFLYWX_FILE"));
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
			this->showHelp(argv[0]);
			exit(EXIT_FAILURE);
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
				strcpy(this->url, argv[++i]);
			}
			else if (currentArg == "--icao") {
				strcpy(this->icaoCode, argv[++i]);
			}
			else if (currentArg == "--response") {
				this->response = (strcmp(argv[++i], "raw") == 0) ? FetchUrl::MODE_RAW : FetchUrl::MODE_JSON;
			}
			else if (currentArg == "--apikey") {
				strcpy(this->apikey, argv[++i]);
			}
			else if (currentArg == "--metar") {
				strcpy(this->metarString, argv[++i]);
			}
			else if (currentArg == "--hours") {
				this->hours = std::stoi(argv[++i]);
			}
		}
		else {
			strcpy(this->filename, argv[i]);
		}
	}
}
