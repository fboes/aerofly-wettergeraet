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

/**
 * Convert a METAR string into a structured object.
 */
class MetarParser
{
public:
	// ICAO code of METAR report
	std::string icao = "";

	// Time and date of observation
	MetarTime observed;

	// Wind direction and strength
	MetarWind wind;

	// Range of visibility
	MetarVisibility visibility;

	// Extra weather conditions like rain, snow
	std::vector<std::string> conditions;

	// 0..2 cloud layers
	MetarCloud clouds[3];

	// Uppermost cloud layer
	MetarCloud * ceiling = NULL;

	// Temperature, dewpoint and humidity
	MetarTemperature temperature;

	// Pressure
	MetarBarometer barometer;

	MetarParser(std::string metarString = "");
	~MetarParser();

	// Populate class variables with data from metarString.
	// Throws std::invalid_argument in case of errors.
	bool convert(std::string metarString);

	// Return flight category like "VFR", "IFR".
	std::string getFlightCategory();
};

