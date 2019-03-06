#pragma once
#include <string>
#include <map>
#include <vector>

/**
 * Convert a METAR string into a structured object.
 */
class MetarParserSimple
{
public:
	struct MetarTimeSimple {
		// Four-digit year
		int year;

		// 1..12
		int month;

		// 1..31
		int day;

		// 0..23
		int hours = 0;

		// 0..59
		int minutes = 0;
	};

	struct MetarWindSimple {
		// 0..359
		unsigned int degrees = 0;

		// 0..359
		unsigned int degreesFrom = 0;

		// 0..359
		unsigned int degreesTo = 0;

		double speedKts = 0;

		double gustKts = 0;
	};

	// Represents a cloud layer
	struct MetarCloudSimple {
		std::string code = "CLR";

		// 0..8
		unsigned short densityMinimum = 0;

		// 0..8
		unsigned short densityMaximum = 0;

		// in feet above ground level
		unsigned long baseFeetAgl = 0;
	};

	// ICAO code of METAR report
	std::string icao = "";

	// Time of observation
	MetarTimeSimple observed;

	// Wind strength and direction
	MetarWindSimple wind;

	// Range of visibility
	double visibilityMeters = 0;

	// Extra weather conditions like rain, snow
	std::vector<std::string> conditions;

	// 0..2 cloud layers:
	MetarCloudSimple clouds[3];

	// Uppermost cloud layer
	MetarCloudSimple * ceiling = NULL;

	// Temperature in °C
	double temperatureCelsius;

	// Temperature in °C
	double dewpointCelsius;

	// Pressure in kilopascal
	double barometerKpa;

	MetarParserSimple(std::string metarString = "");
	~MetarParserSimple();

	// Populate class variables with data from metarString.
	// Throws std::invalid_argument in case of errors.
	bool convert(std::string metarString);

	// Return flight category like "VFR", "IFR"
	std::string getFlightCategory();

	// Get humidity in percent 0..1
	double getHumidity();

	void setDate(short day = -1, short hours = -1, short minutes = -1);
};

