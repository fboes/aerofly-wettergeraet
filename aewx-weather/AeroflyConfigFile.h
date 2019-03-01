#pragma once
#include <string>
#include "AeroflyWeather.h"
#include <tuple>

class AeroflyConfigFile
{
private:
	std::string filename;
	std::string fileBuffer;

	void setValue(std::string &subject, std::string key, std::string value, std::string keyGroup = "");
	void setValue(std::string &subject, std::string key, double value, std::string keyGroup = "");
	std::string getValue(std::string &subject, std::string key, std::string keyGroup = "");

public:
	AeroflyConfigFile(std::string filename = "%USERPROFILE%\\Documents\\Aerofly FS 2\\main.mcf");
	~AeroflyConfigFile();

	bool load();
	bool save();

	std::string getFilename();
	std::string getFileBuffer();

	void setDate(unsigned int year, unsigned int month, unsigned int day);
	std::tuple <unsigned int, unsigned int, unsigned int> getDate();

	void setTime(double hours);
	double getTime();

	void setWind(double percent, unsigned int degrees);
	std::tuple <double, unsigned int> getWind();

	void setTurbulence(double percent);
	double getTurbulence();

	void setThermalActivity(double percent);
	double getThermalActivity();

	void setVisibility(double percent);
	double getVisibility();

	void setCloud(unsigned short index, double heightPercent, double densityPercent);
	std::tuple <double, double> getCloud(unsigned short index);

	std::tuple <std::string, std::string> getFlightplan();

	void setFromAeroflyObject(const AeroflyWeather& aerofly);
	void getToAeroflyObject(AeroflyWeather& aerofly);
};
