#include "pch.h"
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
		+ "                       If this contains 'DEP', ICAO code will be fetched\n"
		+ "                       from Aerofly FS 2 flightplan departure airport.\n"
		+ "                       If this contains 'ARR', ICAO code will be fetched\n"
		+ "                       from Aerofly FS 2 flightplan arrival airport.\n"
		+ "    --apikey <APIKEY>  Sent HTTP header 'X-API-Key' set to <APIKEY>.\n"
		+ "    --response <TYPE>  How to interpret HTTP response. 'json' is default.\n"
		+ "                       Set this to 'raw' if the response is plain text.\n"
		+ "                       Set this to 'json' if the response is JSON object.\n"
		+ "    --metar <METAR>    Supply a valid METAR code enclosed in '\"'.\n"
		+ "                       This will disable HTTP fetching.\n"
		+ "                       If this is set to '?' the value will be asked for.\n"
		+ "    --dry-run          Do not save 'main.mcf'\n"
		+ "    --quiet            No console output\n"
		+ "    --verbose          Show debug output\n"
		+ "    --help             Display this help and exit\n";
}

Argumentor::Argumentor(int argc, char* argv[])
{
	if (getenv("AEROFLYWX_URL") != "") {
		this->url = getenv("AEROFLYWX_URL");
	}
	if (getenv("AEROFLYWX_APIKEY") != "") {
		this->apikey = getenv("AEROFLYWX_APIKEY");
	}
	if (getenv("AEROFLYWX_RESPONSE") != "") {
		this->response = ( getenv("AEROFLYWX_RESPONSE") == "raw") ? FetchUrl::MODE_RAW : FetchUrl::MODE_JSON;
	}

	std::string currentArg = "";
	for (int i = 1; i < argc; ++i) {
		currentArg = std::string(argv[i]);

		if (currentArg == "--help") {
			this->showHelp(argv[0]);
			exit(EXIT_FAILURE);
		}
		else if (currentArg == "--url") {
			this->url = (i + 1 < argc) ? std::string(argv[++i]) : this->url;
		}
		else if (currentArg == "--icao") {
			this->icaoCode = (i + 1 < argc) ? std::string(argv[++i]) : this->icaoCode;
		}
		else if (currentArg == "--response") {
			this->response = (i + 1 < argc && std::string(argv[++i]) == "raw") ? FetchUrl::MODE_RAW : FetchUrl::MODE_JSON;
		}
		else if (currentArg == "--apikey") {
			this->apikey = (i + 1 < argc) ? std::string(argv[++i]) : this->apikey;
		}
		else if (currentArg == "--metar") {
			this->metarString = (i + 1 < argc) ? std::string(argv[++i]) : this->metarString;
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
		else {
			this->filename = argv[i];
		}
	}
}


Argumentor::~Argumentor()
{
}
