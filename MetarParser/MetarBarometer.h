#pragma once

/**
 * Representation of pressure data. Will automatically do conversion into different units.
 * As per convetion you should not change class variables, but use the corresponding
 * setters.
 */
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
