#pragma once
#include "MetarParserSimple.h"
#include "FetchUrl.h"

/**
 * This class represents weather values in Aerofly FS 2. There are multiple methods
 * supplied for changing value in Aerofly FS 2, the most prominent being "setFromMetarString".
 *
 * This class also comes with a setter by fetching value from an URL, and a setter
 * which interprets a MetarParser object.
 */
class AeroflyWeather
{
private:
	double makeGust(double gustSpeed, char const conditions[4][6]);

public:
	unsigned short maxCloudsDensity = 8; // as given in METAR cloud density
	unsigned long maxCloudsHeight = 40000; // ft = 100% Aerofly
	double maxWindSpeed = 16;   // kt = 100% Aerofly
	unsigned long maxVisibility = 15000; // m = 100% Aerofly
	double maxTurbulence = 20;  // kt as delta to current wind
	double minTemperature = 5;  // °C => 0% Aerofly
	double maxTemperature = 30; // °C => 100% Aerofly
	double hourOffset = 0.0;
	bool noRandom = false;

	// Represents a cloud layer
	struct AeroflyCloud {
		double density = 0; // 0.0..1.0
		double height = 0; // 0.0..1.0
	};


	AeroflyCloud clouds[3];   // "cumulus", "cumulus_mediocris", "cirrus"
	double visibility = 1.0;  // 0.0..1.0
	unsigned int year = 2018; // Four-digit-year
	unsigned int month = 5;   // 1..12
	unsigned int day = 5;     // 1..31
	double hours = 0.0;       // 0.0..23.99
	unsigned int windDirection = 0; // 0..359
	double windStrength = 0.0;    // 0.0..1.0
	double windTurbulence = 0.0;  // 0.0..1.0
	double thermalActivity = 0.0; // 0.0..1.0
	char nearestAirport[8] = "";  // ICAO airport code of METAR report

	AeroflyWeather();
	~AeroflyWeather();

	// Change time supplied from outside
	void setHourOffset(double hourOffset);
	void setDate(unsigned int year, unsigned int month, unsigned int day);
	void setTime(unsigned int hours, unsigned int minutes);
	void setWind(double kts, unsigned int degrees);
	void setTurbulence(double windSpeed, double gustSpeed, unsigned int degreesFrom, unsigned int degreesTo, char const conditions[4][6]);
	void setThermalActivity(double celsius);
	void setVisibility(unsigned long meters);
	void setNearestAirport(char const nearestAirport[8]);

	// Set cloud with index 0..2. Density is between 0..8.
	void setCloud(unsigned short index, double baseFeetAgl, unsigned short densityMinimum, unsigned short densityMaximum);

	void unsetCloud(unsigned short index);

	// Set cloud with index 0..2. Density is between 0.0..1.0.
	void setCloudPercent(unsigned short index, double baseFeetAgl, double percent);

	// Convert wind percent into kts
	double getWindKts();

	// Get cloud height in ft
	double getCloudHeightFt(unsigned short index);

	double getVisbilityMeters();

	// Populate all values from a MetarParser object
	void setFromMetar(const MetarParserSimple& metar);

	// Convert METAR information into Aerofly values
	bool setFromMetarString(std::string metarString);

	// Fetch METAR from URL and convert it into Aerofly values
	bool setFromMetarUrl(std::string url, unsigned short fetchMode = FetchUrl::MODE_RAW, std::string apiKey = "");

	// Fetch METAR from URL and convert it into Aerofly values
	bool setFromMetarUrl(std::string url, std::string icaoCode, unsigned short fetchMode = FetchUrl::MODE_RAW, std::string apiKey = "");
};
