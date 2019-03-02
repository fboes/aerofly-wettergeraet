#pragma once
#include <string>
#include <vector>
#include "MetarParser.h"
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
	unsigned short maxCloudsDensity = 8;
	unsigned long maxCloudsHeight = 40000; // ft
	double maxWindSpeed = 40;   // kt
	unsigned long maxVisibility = 20000; // m
	double maxTurbulence = 20;  // kt
	double minTemperature = 5;  // °C
	double maxTemperature = 30; // °C
	double hourOffset = 0.0;
	bool noRandom = false;

	double makeGust(double gustSpeed, std::vector<std::string> conditions);

public:
	double cloudDensity[3]{ 0.0, 0.0, 0.0 }; // 0.0..1.0
	double cloudHeight[3]{ 0.0, 0.0, 0.0 };  // 0.0..1.0
	double visibility = 1.0;  // 0.0..1.0
	unsigned int year = 2018; // Four-digit-year
	unsigned int month = 5;   // 1..12
	unsigned int day = 5;     // 1..31
	double hours = 0.0;       // 0.0..23.99
	unsigned int windDirection = 0; // 0..359
	double windStrength = 0.0;    // 0.0..1.0
	double windTurbulence = 0.0;  // 0.0..1.0
	double thermalActivity = 0.0; // 0.0..1.0

	AeroflyWeather();
	~AeroflyWeather();

	void setDate(unsigned int year, unsigned int month, unsigned int day);
	void setTime(unsigned int hours, unsigned int minutes);
	void setWind(double kts, unsigned int degrees);
	void setTurbulence(double windSpeed, double gustSpeed, unsigned int degreesFrom, unsigned int degreesTo, std::vector<std::string> conditions);
	void setThermalActivity(double celsius);
	void setVisibility(unsigned long meters);

	// Set cloud with index 0..2. Density is between 0..8.
	void setCloud(unsigned short index, double baseFeetAgl, unsigned short densityMinimum, unsigned short densityMaximum);

	// Populate all values from a MetarParser object
	void setFromMetar(const MetarParser& metar);

	// Convert METAR information into Aerofly values
	void setFromMetarString(std::string metarString);

	// Fetch METAR from URL and convert it into Aerofly values
	void setFromMetarUrl(std::string url, unsigned short fetchMode = FetchUrl::MODE_RAW, std::string apiKey = "");

	// Fetch METAR from URL and convert it into Aerofly values
	void setFromMetarUrl(std::string url, std::string icaoCode, unsigned short fetchMode = FetchUrl::MODE_RAW, std::string apiKey = "");
};
