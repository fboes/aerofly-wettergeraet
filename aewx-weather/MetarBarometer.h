#pragma once
class MetarBarometer
{
public:
	double inhg; // Pressure in inch of mercury
	double kpa;  // Pressure in kilopascal
	double mb;   // Pressure in millibar

	MetarBarometer(double kpa = 0);
	~MetarBarometer();

	void setKpa(double kpa);
	void setInhg(double inhg);
};

