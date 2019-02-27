#pragma once
#include <string>
#include "AeroflyWeather.h"

class AeroflyConfigFile
{
private:
	std::string filename;
public:
	AeroflyConfigFile(std::string filename = "%USERPROFILE%\\Documents\\Aerofly FS 2\\main.mcf");
	~AeroflyConfigFile();

	bool load();
	bool save();

	std::string getFilename();

	void setDate(unsigned int year, unsigned int month, unsigned int day);
	void setTime(double hours);
	void setWind(double percent, unsigned int degrees);
	void setTurbulence(double percent);
	void setThermalActivity(double percent);
	void setVisibility(double percent);
	void setCloud(unsigned short index, double heightPercent, double densityPercent);

	void setFromAeroflyObject(AeroflyWeather aerofly);
};
