#pragma once
class MetarTemperature
{
protected:
	void makeHumidity();

public:
	double degreesCelsius;
	double degreesFahrenheit;

	double dewpointCelsius;
	double dewpointFahrenheit;

	double humidity; // Calculated from temperature and dewpoint

	MetarTemperature(double degreesCelsius = 0, double dewpointCelsius = 0);
	~MetarTemperature();

	void setTemperature(double degreesCelsius);
	void setDewpoint(double dewpointCelsius);
};

