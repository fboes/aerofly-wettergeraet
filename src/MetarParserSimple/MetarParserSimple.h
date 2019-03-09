#pragma once
#include <string>
#include <map>
#include <vector>

#ifdef METARPARSERSIMPLE_EXPORTS
#	define METARPARSERSIMPLE_API __declspec(dllexport)
#	define EXPIMP_TEMPLATE
#else
#	define METARPARSERSIMPLE_API __declspec(dllimport)
#	define EXPIMP_TEMPLATE extern
#endif

//EXPIMP_TEMPLATE template class METARPARSERSIMPLE_API std::vector<int>;
//EXPIMP_TEMPLATE template class METARPARSERSIMPLE_API std::vector<char>;

/**
 * Convert a METAR string into a structured object.
 * @see https://stackoverflow.com/questions/16419318/one-way-of-eliminating-c4251-warning-when-using-stl-classes-in-the-dll-interface
 */
class METARPARSERSIMPLE_API MetarParserSimple
{
private:
	// Keep time and date in bounds
	void fixTimeDate();

	int fixDegrees(int degrees);

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
	char * icao;

	// Time of observation
	MetarTimeSimple observed;

	// Wind strength and direction
	MetarWindSimple wind;

	// Range of visibility
	double visibilityMeters = 0;

	// Extra weather conditions like rain, snow
	char * conditions[6];

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

	// Set date / time to current UTC time, but allow for modification.
	void setDate(short day = -1, short hours = -1, short minutes = -1);

	// Change parsed time by hoursOffset.
	void addHours(int hoursOffset);

	// Test if a certain condition is contained in METAR information.
	bool hasCondition(std::string testCondition);
};

