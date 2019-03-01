#include "pch.h"
#include "AeroflyConfigFile.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>

void AeroflyConfigFile::setValue(std::string &subject, std::string key, std::string value, std::string keyGroup)
{
	std::string regSearch = "(";
	if (keyGroup != "") {
		regSearch += "            <\\[\\S+";
	}
	regSearch += "\\]\\[" + key + "\\])\\[[^\\]]*\\]";
	subject = std::regex_replace(
		subject,
		std::regex(regSearch),
		"$1[" + value + "]",
		std::regex_constants::format_default
	);
}

void AeroflyConfigFile::setValue(std::string &subject, std::string key, double value, std::string keyGroup)
{
	this->setValue(subject, key, std::to_string(value), keyGroup);
}

AeroflyConfigFile::AeroflyConfigFile(std::string filename)
{
	filename = std::regex_replace(
		filename,
		std::regex("%USERPROFILE%", std::regex_constants::icase),
		getenv("USERPROFILE")
	);
	this->filename = filename;
}

AeroflyConfigFile::~AeroflyConfigFile()
{
}

// Load this->filename into this->fileBuffer
bool AeroflyConfigFile::load()
{
	std::ifstream cfgFile(this->filename, std::ios::in | std::ios::binary);
	if (cfgFile)
	{
		std::ostringstream contents;
		contents << cfgFile.rdbuf();
		cfgFile.close();
		this->fileBuffer = contents.str();
		return true;
	}
	throw(errno);
}

// Save this->fileBuffer into this->filename
bool AeroflyConfigFile::save()
{
	std::ofstream cfgFile(this->filename);
	if (cfgFile.is_open())
	{
		cfgFile << this->fileBuffer;
		cfgFile.close();
		return true;
	}
	throw(errno);
}

std::string AeroflyConfigFile::getFilename()
{
	return this->filename;
}

std::string AeroflyConfigFile::getFileBuffer()
{
	return this->fileBuffer;
}

void AeroflyConfigFile::setDate(unsigned int year, unsigned int month, unsigned int day)
{
	this->setValue(this->fileBuffer, "time_year", std::to_string(year), "tm_time_utc");
	this->setValue(this->fileBuffer, "time_month", std::to_string(month), "tm_time_utc");
	this->setValue(this->fileBuffer, "time_day", std::to_string(day), "tm_time_utc");
}

void AeroflyConfigFile::setTime(double hours)
{
	this->setValue(this->fileBuffer, "time_hours", hours, "tm_time_utc");
}

void AeroflyConfigFile::setWind(double percent, unsigned int degrees)
{
	this->setValue(this->fileBuffer, "strength", percent, "tmsettings_wind");
	this->setValue(this->fileBuffer, "direction_in_degree", std::to_string(degrees), "tmsettings_wind");
}

void AeroflyConfigFile::setTurbulence(double percent)
{
	this->setValue(this->fileBuffer, "turbulence", percent, "tmsettings_wind");
}

void AeroflyConfigFile::setThermalActivity(double percent)
{
	this->setValue(this->fileBuffer, "thermal_activity", percent,"tmsettings_wind");
}

void AeroflyConfigFile::setVisibility(double percent)
{
	this->setValue(this->fileBuffer, "visibility", percent);
}

void AeroflyConfigFile::setCloud(unsigned short index, double heightPercent, double densityPercent)
{
	std::string cloudName = "";
	switch (index) {
	case 1:
		cloudName = "cumulus";
		break;
	case 2:
		cloudName = "cumulus_mediocris";
		break;
	default:
		cloudName = "cirrus";
		break;
	}
	this->setValue(this->fileBuffer, cloudName + "_height", heightPercent, "tmsettings_clouds");
	this->setValue(this->fileBuffer, cloudName + "_density", densityPercent, "tmsettings_clouds");
}

void AeroflyConfigFile::setFromAeroflyObject(const AeroflyWeather& aerofly)
{
	this->setDate(aerofly.year, aerofly.month, aerofly.day);
	this->setTime(aerofly.hours);
	this->setWind(aerofly.windStrength, aerofly.windDirection);
	this->setTurbulence(aerofly.windTurbulence);
	this->setThermalActivity(aerofly.thermalActivity);
	this->setVisibility(aerofly.visibility);

	for (int i = 0; i < 3; ++i) {
		this->setCloud(i, aerofly.cloudHeight[i], aerofly.cloudDensity[i]);
	}
}
