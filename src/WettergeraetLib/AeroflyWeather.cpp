#include "stdafx.h"
#include <algorithm>
#include <time.h>
#include "AeroflyWeather.h"
#include "BoShed.h"
#include "FetchUrl.h"

double AeroflyWeather::makeGust(double windSpeed, char const conditions[4][6])
{
	auto gustSpeed = windSpeed;

	// Get gusts from weather
	unsigned int sizeOfConditions = 6;
	for (unsigned int i = 0; i <= sizeOfConditions; i++) {
			if (strcmp(conditions[i], "WS") == 0) {
				// Wind Shear
				gustSpeed = std::max(windSpeed + 10.0, 45.0);
			}
			else if (strcmp(conditions[i], "SS") == 0 || strcmp(conditions[i], "DS") == 0) {
				// Storm
				gustSpeed = std::max(windSpeed + 5.0, 40.0);
			}
			else if (strcmp(conditions[i], "FC") == 0) {
				// Funnel cloud(s) (tornado or waterspout)
				gustSpeed = std::max(windSpeed + 20.0, 90.0);
			}
			else if (strcmp(conditions[i], "SQ") == 0) {
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
void AeroflyWeather::setTurbulence(double windSpeed, double gustSpeed, unsigned int degreesFrom, unsigned int degreesTo, char const conditions[4][6])
{
	double degreesRange = std::abs((degreesTo - degreesFrom) / 360.0);

	// Limit variable wind bearing impact relative to wind force
	if (windSpeed / 10 < degreesRange) {
		degreesRange = windSpeed / 10;
	}

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

void AeroflyWeather::setPressureHpa(double hpa)
{
	this->pressure = BoShed::percent(
		(hpa - this->minPressure) / (this->maxPressure - this->minPressure), true
	);
}

void AeroflyWeather::setVisibility(unsigned long meters)
{
	const unsigned long maxMiles = (long)(10 * 1609.344);
	if (this->maxVisibility > 9999 && (meters == 9999 || meters == 10000)) {
		// because meters cannot be greater than 9999
		// but miles can reach up to 10sm
		// set 9999 meters to 10 sm
		meters = maxMiles;
	}
	this->visibility = BoShed::percent((double)meters / (double)this->maxVisibility, true);
	if (meters <= maxMiles && this->maxVisibility > maxMiles) {
		// max visibility range cannot be reached, because in METAR there is only <= 10 sm
		// Interpolate the last part
		this->visibility += (1.0 - ((double)maxMiles / (double)this->maxVisibility)) * std::pow((meters / maxMiles), 8);
	}
}

void AeroflyWeather::setNearestAirport(char const nearestAirport[8])
{
	strcpy_s(this->nearestAirport, 8, nearestAirport);
}

void AeroflyWeather::setHourOffset(double hourOffset)
{
	this->hourOffset = hourOffset;
}

void AeroflyWeather::setCloud(unsigned short index, double baseFeetAgl, unsigned short densityMinimum, unsigned short densityMaximum)
{
	srand((int)time(NULL));
	if (densityMinimum > 0) {
		densityMinimum--;
	}

	auto density = (densityMinimum + (
		(rand() % 100 / 100.0) * (densityMaximum - densityMinimum)
		)) / this->maxCloudsDensity;
	this->setCloudPercent(index, baseFeetAgl, density);
}

void AeroflyWeather::unsetCloud(unsigned short index)
{
	this->setCloudPercent(index, 0, 0);
}

void AeroflyWeather::setCloudPercent(unsigned short index, double baseFeetAgl, double percent)
{
	this->clouds[index].density = percent;
	this->clouds[index].height = baseFeetAgl / this->maxCloudsHeight;
}

double AeroflyWeather::getWindKts()
{
	// f(x) = 8 * (x + x^2)
	return 8 * (this->windStrength + this->windStrength * this->windStrength);
}

double AeroflyWeather::getCloudHeightFt(unsigned short index)
{
	return this->clouds[index].height * this->maxCloudsHeight;
}

double AeroflyWeather::getVisbilityMeters()
{
	return this->visibility * this->maxVisibility;
}

double AeroflyWeather::getPressureHpa()
{
	return this->minPressure + ((this->maxPressure - this->minPressure) *  this->pressure);
}

void AeroflyWeather::setFromMetar(const MetarParserSimple& metar)
{
	this->setNearestAirport(metar.icao);
	//this->setDate(metar.observed.year, metar.observed.month, metar.observed.day);
	//this->setTime(metar.observed.hours, metar.observed.minutes);
	this->setWind(metar.wind.speedKts, metar.wind.degrees);
	this->setTurbulence(metar.wind.speedKts, metar.wind.gustKts, metar.wind.degreesFrom, metar.wind.degreesTo, metar.conditions);
	this->setThermalActivity(metar.temperatureCelsius);
	this->setPressureHpa(metar.barometerKpa * 10.0);
	this->setVisibility((int)metar.visibilityMeters);

	// If not all cloud layers are sets, try to set cirrus cloud at a height above 10,000 ft
	if (metar.numberOfClouds == 1) {
		bool chooseLowerCloud = (metar.clouds[0].baseFeetAgl < 10000);
		this->setCloud(chooseLowerCloud ? 0 : 2, metar.clouds[0].baseFeetAgl, metar.clouds[0].densityMinimum, metar.clouds[0].densityMaximum);
		this->unsetCloud(1);
		this->unsetCloud(chooseLowerCloud ? 2 : 0);
	}
	else if (metar.numberOfClouds == 2) {
		bool chooseLowerCloud = (metar.clouds[1].baseFeetAgl < 10000);
		this->setCloud(0, metar.clouds[0].baseFeetAgl, metar.clouds[0].densityMinimum, metar.clouds[0].densityMaximum);
		this->setCloud(chooseLowerCloud ? 1 : 2, metar.clouds[1].baseFeetAgl, metar.clouds[1].densityMinimum, metar.clouds[1].densityMaximum);
		this->unsetCloud(chooseLowerCloud ? 2 : 1);
	} else {
		for (int i = 0; i < 3; ++i) {
			this->setCloud(i, metar.clouds[i].baseFeetAgl, metar.clouds[i].densityMinimum, metar.clouds[i].densityMaximum);
		}
	}
}

bool AeroflyWeather::setFromMetarString(std::string metarString)
{
	MetarParserSimple metar(metarString);
	this->setFromMetar(metar);
	return true;
}

bool AeroflyWeather::setFromMetarUrl(std::string url, unsigned short fetchMode, std::string apiKey)
{
	FetchUrl urlFetcher;
	auto metarString = urlFetcher.fetch(url, fetchMode, apiKey);
	return this->setFromMetarString(metarString);
}

bool AeroflyWeather::setFromMetarUrl(std::string url, std::string icaoCode, std::string date, short fetchMode, std::string apiKey)
{
	FetchUrl urlFetcher;
	auto metarString = urlFetcher.fetch(url, icaoCode, date, fetchMode, apiKey);
	return this->setFromMetarString(metarString);
}

