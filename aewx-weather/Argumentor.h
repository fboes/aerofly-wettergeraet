#pragma once
#include <string>
#include "FetchUrl.h"

/**
 * Parsing CLI parameters
 * @see http://www.cplusplus.com/articles/DEN36Up4/
 */
class Argumentor
{
public:
	std::string icaoCode = "";

	std::string url = "http://avwx.rest/api/metar/XXXX?options=&format=json&onfail=cache";

	unsigned short response = FetchUrl::MODE_JSON;

	std::string apikey = "";

	std::string metarString = "";

	bool isDryRun = false;

	// 0: quiet; 1: normal; 2: verbose
	unsigned short verbosity = 1;

	// Location of main.mcf
	std::string filename = "%USERPROFILE%\\Documents\\Aerofly FS 2\\main.mcf";

	// Show help output for CLI parameters
	std::string showHelp(std::string cmd);

	Argumentor(int argc, char* argv[]);
	~Argumentor();
};

