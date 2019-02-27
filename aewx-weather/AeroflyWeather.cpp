#include "pch.h"
#include <algorithm>
#include "AeroflyWeather.h"
#include "BoShed.h"
#include "FetchUrl.h"

/**
 * @class AeroflyWeather
 * This class represents weather values in Aerofly FS 2. There are multiple methods
 * supplied for changing value in Aerofly FS 2, the most prominent being "setFromMetarString".
 *
 * This class also comes with a setter by fetching value from an URL, and a setter
 * which interprets a MetarParser object.
 */

double AeroflyWeather::makeGust(double windSpeed, std::vector<std::string> conditions)
{
	auto gustSpeed = windSpeed;

	// Get gusts from weather
	for (auto const& condition : conditions) {
		if (condition == "WS") {
			// Wind Shear
			gustSpeed = std::max(windSpeed + 10.0, 45.0);
		}
		else if (condition == "SS" || condition == "DS") {
			// Storm
			gustSpeed = std::max(windSpeed + 5.0, 40.0);
		}
		else if (condition == "FC") {
			// Funnel cloud(s) (tornado or waterspout)
			gustSpeed = std::max(windSpeed + 20.0, 90.0);
		}
		else if (condition == "SQ") {
			// Squalls
			gustSpeed = windSpeed += 10;
		}
	}

	// Make some extra gusts (Gusts are only reported if it is above the mean wind speed by 10kts or more)
	gustSpeed += std::sqrt(gustSpeed);
	return gustSpeed;
}

AeroflyWeather::AeroflyWeather()
{
}


AeroflyWeather::~AeroflyWeather()
{

}

void AeroflyWeather::setDate(unsigned int year, unsigned int month, unsigned int day)
{
	this->year = year;
	this->month = BoShed::rotatingValue(month, 12, 1);
	this->day = BoShed::rotatingValue(day, 31, 1);
}

void AeroflyWeather::setTime(unsigned int hours, unsigned int minutes)
{
	this->hours = (double)hours + this->hourOffset + ((double)minutes / 60.0);
}

void AeroflyWeather::setWind(double kts, unsigned int degrees)
{
	// For 16kts <=> 100 %
	// f(x) = 8 * (x + x^2)
	// f(y) = sqrt((y / 8) + 0.25) - 0.5
	double combinedWindForce = std::sqrt((kts / (this->maxWindSpeed / 2)) + 0.25) - 0.5;
	this->windDirection = BoShed::rotatingValue(degrees, 359);
	this->windStrength = BoShed::percent(combinedWindForce);
}

void AeroflyWeather::setTurbulence(double windSpeed, double gustSpeed, unsigned int degreesFrom, unsigned int degreesTo, std::vector<std::string> conditions)
{
	const double degreesRange = std::abs((degreesTo - degreesFrom) / 360.0);

	if (gustSpeed == windSpeed) {
		// Spice up gust winds
		gustSpeed = this->makeGust(gustSpeed, conditions);
	}

	this->windTurbulence = BoShed::percent(
		std::max(degreesRange, (gustSpeed - windSpeed) / this->maxTurbulence),
		true
	);
}

void AeroflyWeather::setThermalActivity(double celsius)
{
	this->thermalActivity = BoShed::percent(
		(celsius - this->minTemperature) / (this->maxTemperature - this->minTemperature), true
	);
}

void AeroflyWeather::setVisibility(unsigned long meters)
{
	const unsigned long maxMiles = 10 * 1609;
	meters = std::min(maxMiles, meters);
	if (this->maxVisibility > 9999 && (meters == 9999 || meters == 10000)) {
		// because meters cannot be greater than 9999
		// but miles can reach up to 10sm
		// set 9999 meters to 10 sm
		meters = maxMiles;
	}
	this->visibility = BoShed::percent((double)meters / (double)this->maxVisibility, true);
	if (this->maxVisibility > maxMiles) {
		// max visibility range cannot be reached, because in METAR there is only <= 10 sm
		// Interpolate the last part
		this->visibility += (1.0 - ((double)maxMiles / (double)this->maxVisibility)) * std::pow((meters / maxMiles), 8);
	}
}

// Set cloud 0..2
void AeroflyWeather::setCloud(unsigned short index, double baseFeetAgl, double densityPercent)
{
	this->cloudHeight[index] = baseFeetAgl / this->maxCloudsHeight;
	this->cloudDensity[index] = densityPercent;
}

// Populate all values from a MetarParser object
void AeroflyWeather::setFromMetar(MetarParser metar)
{
	this->setDate(metar.observed.year, metar.observed.month, metar.observed.day);
	this->setTime(metar.observed.hours, metar.observed.minutes);
	this->setWind(metar.wind.speedKts, metar.wind.degrees);
	this->setTurbulence(metar.wind.speedKts, metar.wind.gustKts, metar.wind.degreesFrom, metar.wind.degreesTo, metar.conditions);
	this->setThermalActivity(metar.temperature.degreesCelsius);
	this->setVisibility((int)metar.visibility.meters);
	for (int i = 0; i < 3; ++i) {
		this->setCloud(i, metar.clouds[i].baseFeetAgl, metar.clouds[i].getRandomDensityPercent());
	}
}

// Convert METAR information into Aerofly values
void AeroflyWeather::setFromMetarString(std::string metarString)
{
	MetarParser metar(metarString);
	this->setFromMetar(metar);
}

// Fetch METAR from URL and convert it into Aerofly values
void AeroflyWeather::setFromMetarUrl(std::string url, unsigned short fetchMode, std::string apiKey)
{
	FetchUrl urlFetcher;
	auto metarString = urlFetcher.fetch(url, fetchMode, apiKey);
	this->setFromMetarString(metarString);
}

// Fetch METAR from URL and convert it into Aerofly values
void AeroflyWeather::setFromMetarUrl(std::string url, std::string icaoCode, unsigned short fetchMode, std::string apiKey)
{
	FetchUrl urlFetcher;
	auto metarString = urlFetcher.fetch(url, icaoCode, fetchMode, apiKey);
	this->setFromMetarString(metarString);
}
