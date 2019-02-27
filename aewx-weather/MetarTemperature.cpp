#include "pch.h"
#include "MetarTemperature.h"
#include "Convert.h"
#include <math.h>

void MetarTemperature::makeHumidity()
{
	this->humidity = exp(
		(17.625 * this->dewpointCelsius) / (243.04 + this->dewpointCelsius)
	) / exp(
		(17.625 * this->degreesCelsius) / (243.04 + this->degreesCelsius)
	);
}

MetarTemperature::MetarTemperature(double degreesCelsius, double dewpointCelsius)
{
	this->setTemperature(degreesCelsius);
	this->setDewpoint(dewpointCelsius);
}

MetarTemperature::~MetarTemperature()
{
}

void MetarTemperature::setTemperature(double degreesCelsius)
{
	this->degreesCelsius = degreesCelsius;
	this->degreesFahrenheit = Convert::celsiusToFahrenheit(degreesCelsius);
	this->makeHumidity();
}

void MetarTemperature::setDewpoint(double dewpointCelsius)
{
	this->dewpointCelsius = dewpointCelsius;
	this->dewpointFahrenheit = Convert::celsiusToFahrenheit(dewpointCelsius);
	this->makeHumidity();
}
