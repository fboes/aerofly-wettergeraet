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
	std::string showHelpOption(std::string label, std::string description, const int indentLength = 2, const int columnLength = 21);
	std::string wordwrap(std::string candidate, const int maxLineLength = 82, std::string wrapper = "\n");


public:
	static const char* APP_VERSION;
	static const char* APP_TARGET;

	char icaoCode[8] = "";

	char url[512] = "https://avwx.rest/api/metar/XXXX?options=&format=json&onfail=cache";

	unsigned short response = FetchUrl::MODE_JSON;

	char apikey[64] = "";

	char metarString[512] = "";

	// Initially loaded METAR file
	char metarfile[512] = "";

	int hours = 0;

	bool isDryRun = false;
	bool isShowHelp = false;
	bool isShowVersion = false;

	// 0: quiet; 1: normal; 2: verbose
	unsigned short verbosity = 1;

	// Location of main.mcf
	char filename[512] = "%USERPROFILE%\\Documents\\Aerofly FS 2\\main.mcf";

	// Show help output for CLI parameters
	std::string showHelp(std::string cmd);

	// Show version information
	std::string showVersion(std::string appname = "Aerofly Wettergeraet");

	Argumentor();
	~Argumentor();

	void getArgs(int argc, char* argv[]);
};

