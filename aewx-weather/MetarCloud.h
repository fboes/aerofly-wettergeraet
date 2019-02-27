#pragma once
#include <string>

class MetarCloud
{
protected:
	double convertEighthToPercent(double eight, unsigned short extraMax = 0);

public:
	std::string code;              // like "CLR", "OVC"
	unsigned short densityMinimum; // 0..8
	unsigned short densityMaximum; // 0..8
	unsigned long baseFeetAgl;     // in feet above ground level
	unsigned long baseMetersAgl;   // in meters above ground level

	MetarCloud(std::string code = "CLR", unsigned long baseFeetAgl = 0);
	~MetarCloud();

	void setCode(std::string code);
	void setFeetAgl(unsigned long baseFeetAgl);
	double getAverageDensity(unsigned short extraMax = 0);
	double getRandomDensity(unsigned short extraMax = 0);
	double getAverageDensityPercent();
	double getRandomDensityPercent();
	double getMaximumDensityPercent();
};
