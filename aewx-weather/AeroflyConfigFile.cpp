#include "pch.h"
#include "AeroflyConfigFile.h"
#include <iostream>
#include <fstream>
#include <regex>

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

bool AeroflyConfigFile::load()
{
	return false;
}

bool AeroflyConfigFile::save()
{
	return false;
}

std::string AeroflyConfigFile::getFilename()
{
	return this->filename;
}

void AeroflyConfigFile::setDate(unsigned int year, unsigned int month, unsigned int day)
{
}

void AeroflyConfigFile::setTime(double hours)
{
}

void AeroflyConfigFile::setWind(double percent, unsigned int degrees)
{
}

void AeroflyConfigFile::setTurbulence(double percent)
{
}

void AeroflyConfigFile::setThermalActivity(double percent)
{
}

void AeroflyConfigFile::setVisibility(double percent)
{
}

void AeroflyConfigFile::setCloud(unsigned short index, double heightPercent, double densityPercent)
{
}

void AeroflyConfigFile::setFromAeroflyObject(AeroflyWeather aerofly)
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
