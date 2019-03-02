#pragma once

/**
 * Representation of temperature data. Will automatically do conversion into different units.
 * As per convetion you should not change class variables, but use the corresponding
 * setters.
 */
class MetarTemperature
{
protected:
	// Calulate humidity from temeprature and dewpoint.
	void makeHumidity();

public:
	double degreesCelsius;
	double degreesFahrenheit;

	double dewpointCelsius;
	double dewpointFahrenheit;

	// Calculated from temperature and dewpoint. See this->makeHumidity(). Is 0..100
	double humidity;

	MetarTemperature(double degreesCelsius = 0, double dewpointCelsius = 0);
	~MetarTemperature();

	void setTemperature(double degreesCelsius);
	void setDewpoint(double dewpointCelsius);
};

