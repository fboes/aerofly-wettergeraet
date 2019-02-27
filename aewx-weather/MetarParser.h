#pragma once
#include <string>
#include <map>
#include <vector>
#include "MetarTime.h"
#include "MetarWind.h"
#include "MetarVisibility.h"
#include "MetarCloud.h"
#include "MetarTemperature.h"
#include "MetarBarometer.h"

class MetarParser
{
	unsigned short currentCloud = 0;

	public:
		std::string icao = ""; // ICAO code of METAR report
		MetarTime observed;    // Time and date of observation
		MetarWind wind;        // Wind direction and strength
		MetarVisibility visibility; // Range of visibility
		std::vector<std::string> conditions; // Extra weather conditions like rain, snow
		MetarCloud clouds [3];        // 0..2 cloud layers
		MetarCloud * ceiling = NULL;  // Uppermost cloud layer
		MetarTemperature temperature; // Temperature, dewpoint and humidity
		MetarBarometer barometer;     // Pressure

		MetarParser(std::string metarString = "");
		~MetarParser();

		bool convert(std::string metarString);
		std::string getFlightCategory();
};

