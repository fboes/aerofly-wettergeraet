#pragma once
#include "AeroflyWeather.h"
#include <tuple>
#include <fstream>

/**
 * This represents the values of the Aerofly FS2 main configuration file.
 * Values can be read and changed.
 */
class AeroflyConfigFile
{
private:
	std::string filename;
	std::string fileBuffer;
	std::fstream fileStream;

	void setValue(std::string &subject, std::string key, std::string value, std::string keyGroup = "");
	void setValue(std::string &subject, std::string key, double value, std::string keyGroup = "");
	std::string getValue(std::string &subject, std::string key, std::string keyGroup = "");

public:
	AeroflyConfigFile(std::string filename = "%USERPROFILE%\\Documents\\Aerofly FS 2\\main.mcf");
	~AeroflyConfigFile();

	// Load this->filename into this->fileBuffer.
	// If onlyOnce is set to `true`, only load file if it has not been loaded before.
	// Throws std::invalid_argument in case of errors.
	bool load(bool onlyOnce = false);

	// Save this->fileBuffer into this->filename.
	// Throws std::invalid_argument in case of errors.
	bool save();

	void setFilename(std::string filename);

	std::string getFilename();

	std::string getFileBuffer();

	void setDate(unsigned int year, unsigned int month, unsigned int day);

	// Returns year, month, day
	std::tuple <unsigned int, unsigned int, unsigned int> getDate();

	void setTime(double hours);

	double getTime();

	void setWind(double percent, unsigned int degrees);

	// Returns strength, direction_in_degree
	std::tuple <double, unsigned int> getWind();

	void setTurbulence(double percent);

	double getTurbulence();

	void setThermalActivity(double percent);

	double getThermalActivity();

	void setVisibility(double percent);

	double getVisibility();

	void setCloud(unsigned short index, double heightPercent, double densityPercent);

	// Returns height, density
	std::tuple <double, double> getCloud(unsigned short index);

	// Returns Origin, Destination
	std::tuple <std::string, std::string> getFlightplan();

	// Set values from Aerofly object to this->fileBuffer
	void setFromAeroflyObject(const AeroflyWeather& aerofly);

	// Get values fromt this->fileBuffer and paste these to aerofly
	void getToAeroflyObject(AeroflyWeather& aerofly);
};
