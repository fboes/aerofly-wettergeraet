#pragma once
#include <string>

class MetarCloud
{
protected:
	double convertEighthToPercent(double eight);

public:
	std::string code = "CLR";          // like "CLR", "OVC"
	unsigned short densityMinimum = 0; // 0..8
	unsigned short densityMaximum = 0; // 0..8
	unsigned long baseFeetAgl = 0;     // in feet above ground level
	unsigned long baseMetersAgl = 0;   // in meters above ground level

	MetarCloud(std::string code = "CLR", unsigned long baseFeetAgl = 0);
	~MetarCloud();

	void setCode(std::string code);
	void setFeetAgl(unsigned long baseFeetAgl);
	double getAverageDensity(bool extraRange = false);
	double getRandomDensity(bool extraRange = false);
	double getAverageDensityPercent();
	double getRandomDensityPercent();
	double getMaximumDensityPercent();
};
