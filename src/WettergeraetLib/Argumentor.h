#pragma once
#include "FetchUrl.h"

/**
 * Parsing CLI parameters
 * @see http://www.cplusplus.com/articles/DEN36Up4/
 */
class Argumentor
{
private:
	char * getEnv(const char* varName);

public:
	char icaoCode[8] = "";

	char url[512] = "http://avwx.rest/api/metar/XXXX?options=&format=json&onfail=cache";

	unsigned short response = FetchUrl::MODE_JSON;

	char apikey[64] = "";

	char metarString[512] = "";

	int hours = 0;

	bool isDryRun = false;

	// 0: quiet; 1: normal; 2: verbose
	unsigned short verbosity = 1;

	// Location of main.mcf
	char filename[512] = "%USERPROFILE%\\Documents\\Aerofly FS 2\\main.mcf";

	// Show help output for CLI parameters
	std::string showHelp(std::string cmd);

	Argumentor();
	~Argumentor();

	void getArgs(int argc, char* argv[]);
};

