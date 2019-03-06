#pragma once
#include <string>

/**
 * Representation of a single cloud layer. Will automatically do conversion into different units.
 * As per convetion you should not change class variables, but use the corresponding
 * setters.
 */
class MetarCloud
{
protected:
	double convertEighthToPercent(double eight);

public:
	// Cloud code like "CLR", "OVC"
	std::string code = "CLR";

	// 0..8
	unsigned short densityMinimum = 0;

	// 0..8
	unsigned short densityMaximum = 0;

	// in feet above ground level
	unsigned long baseFeetAgl = 0;

	// in meters above ground level
	unsigned long baseMetersAgl = 0;

	// Cloud code like "CLR", "OVC"
	MetarCloud(std::string code = "CLR", unsigned long baseFeetAgl = 0);
	~MetarCloud();

	void setCode(std::string code);
	void setFeetAgl(unsigned long baseFeetAgl);

	// If extraRange is activated, the minimumDensity is reduced by 1 to have some more range
	double getAverageDensity(bool extraRange = false);

	// If extraRange is activated, the minimumDensity is reduced by 1 to have some more range
	double getRandomDensity(bool extraRange = false);

	// Returns a value 0..1.
	double getAverageDensityPercent();

	// Returns a value 0..1.
	double getRandomDensityPercent();

	// Returns a value 0..1.
	double getMaximumDensityPercent();
};
