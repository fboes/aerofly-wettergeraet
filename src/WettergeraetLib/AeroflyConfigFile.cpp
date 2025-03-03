#include "stdafx.h"
#include "AeroflyConfigFile.h"
#include <regex>
#include <iostream>
#include <sstream>

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif


void AeroflyConfigFile::setValue(std::string &subject, std::string key, std::string value, std::string keyGroup)
{
	std::string regSearch = "(";
	if (keyGroup != "") {
		regSearch += keyGroup + "[\\S\\s]+?";
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

std::string AeroflyConfigFile::getValue(std::string &subject, std::string key, std::string keyGroup)
{
	std::smatch match;

	std::string regSearch = "(?:";
	if (keyGroup != "") {
		regSearch += keyGroup + "[\\S\\s]+?";
	}
	regSearch += "\\]\\[" + key + "\\]\\[)([^\\]]*)(?:\\])";

	if (std::regex_search(subject, match, std::regex(regSearch))) {
		return match[1].str();
	}
	return "";
}

std::vector<std::string> AeroflyConfigFile::getValueVector(std::string& subject, std::string key, std::string keyGroup)
{
	std::string value = this->getValue(subject, key, keyGroup).c_str();

	std::vector<std::string> tokens;
	std::istringstream iss(value);
	copy(
		std::istream_iterator<std::string>(iss),
		std::istream_iterator<std::string>(),
		back_inserter(tokens));
	return tokens;
}

AeroflyConfigFile::AeroflyConfigFile(std::string filename)
{
	this->setFilename(filename);
}

AeroflyConfigFile::~AeroflyConfigFile()
{
}

bool AeroflyConfigFile::load(bool onlyOnce)
{
	if (onlyOnce && this->fileBuffer != "") {
		return true;
	}
	this->fileStream.open(this->filename, std::ios::in | std::ios::binary);
	if (this->fileStream.is_open())
	{
		std::ostringstream contents;
		contents << this->fileStream.rdbuf();
		this->fileStream.close();
		this->fileBuffer = contents.str();
		return true;
	}
	throw std::invalid_argument("Could no open " + this->filename + " for reading");
}

bool AeroflyConfigFile::save()
{
	this->fileStream.open(this->filename, std::ios::out | std::ios::binary);
	if (this->fileStream.is_open())
	{
		this->fileStream << this->fileBuffer;
		this->fileStream.close();
		return true;
	}
	throw std::invalid_argument("Could no open " + this->filename + " for writing");
}

void AeroflyConfigFile::setFilename(std::string filename)
{
	char *userprofileDirectory;
	size_t len;
	errno_t err = _dupenv_s(&userprofileDirectory, &len, "USERPROFILE");
	if (!err) {
		filename = std::regex_replace(
			filename,
			std::regex("%USERPROFILE%", std::regex_constants::icase),
			userprofileDirectory
		);
	}
	free(userprofileDirectory);
	this->filename = filename;
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

std::tuple<unsigned int, unsigned int, unsigned int> AeroflyConfigFile::getDate()
{
	return {
		(unsigned int)atof(this->getValue(this->fileBuffer, "time_year", "tm_time_utc").c_str()),
		(unsigned int)atof(this->getValue(this->fileBuffer, "time_month", "tm_time_utc").c_str()),
		(unsigned int)atof(this->getValue(this->fileBuffer, "time_day", "tm_time_utc").c_str())
	};
}

void AeroflyConfigFile::setTime(double hours)
{
	this->setValue(this->fileBuffer, "time_hours", hours, "tm_time_utc");
}

double AeroflyConfigFile::getTime()
{
	return atof(this->getValue(this->fileBuffer, "time_hours", "tm_time_utc").c_str());
}

void AeroflyConfigFile::setWind(double percent, unsigned int degrees)
{
	this->setValue(this->fileBuffer, "strength", percent, "tmsettings_wind");
	this->setValue(this->fileBuffer, "direction_in_degree", std::to_string(degrees), "tmsettings_wind");
}

std::tuple<double, unsigned int> AeroflyConfigFile::getWind()
{
	return {
		(double)atof(this->getValue(this->fileBuffer, "strength", "tmsettings_wind").c_str()),
		(unsigned int)atof(this->getValue(this->fileBuffer, "direction_in_degree", "tmsettings_wind").c_str()),
	};
}

void AeroflyConfigFile::setTurbulence(double percent)
{
	this->setValue(this->fileBuffer, "turbulence", percent, "tmsettings_wind");
}

double AeroflyConfigFile::getTurbulence()
{
	return atof(this->getValue(this->fileBuffer, "turbulence", "tmsettings_wind").c_str());
}

void AeroflyConfigFile::setThermalActivity(double percent)
{
	this->setValue(this->fileBuffer, "thermal_activity", percent, "tmsettings_wind");
}

double AeroflyConfigFile::getThermalActivity()
{
	return atof(this->getValue(this->fileBuffer, "thermal_activity", "tmsettings_wind").c_str());
}

void AeroflyConfigFile::setPressure(double percent)
{
	// TODO, missing in Aerofly FS 2 / Aerofly FS 4
}

double AeroflyConfigFile::getPressure()
{
	return 0.5;
}

void AeroflyConfigFile::setVisibility(double percent)
{
	this->setValue(this->fileBuffer, "visibility", percent);
}

double AeroflyConfigFile::getVisibility()
{
	return atof(this->getValue(this->fileBuffer, "visibility").c_str());
}

void AeroflyConfigFile::setCloud(unsigned short index, double heightPercent, double densityPercent)
{
	std::string cloudName = "";
	switch (index) {
	case 0:
		cloudName = "cumulus";
		break;
	case 1:
		cloudName = "cumulus_mediocris";
		break;
	default:
		cloudName = "cirrus";
		break;
	}
	this->setValue(this->fileBuffer, cloudName + "_height", heightPercent, "tmsettings_clouds");
	this->setValue(this->fileBuffer, cloudName + "_density", densityPercent, "tmsettings_clouds");
}

std::tuple<double, double> AeroflyConfigFile::getCloud(unsigned short index)
{
	std::string cloudName = "";
	switch (index) {
	case 0:
		cloudName = "cumulus";
		break;
	case 1:
		cloudName = "cumulus_mediocris";
		break;
	default:
		cloudName = "cirrus";
		break;
	}
	return {
		(double)atof(this->getValue(this->fileBuffer, cloudName + "_height", "tmsettings_clouds").c_str()),
		(double)atof(this->getValue(this->fileBuffer, cloudName + "_density", "tmsettings_clouds").c_str())
	};
}

AeroflyConfigFile::GeoCoordinates AeroflyConfigFile::getPosition()
{
	std::vector<std::string> value = this->getValueVector(this->fileBuffer, "position", "tmsettings_flight");

	if (value.size() != 3)
	{
		GeoCoordinates position;
		position.longitude = 0.0;
		position.latitude = 0.0;
		return position;
	}

	return this->LonLatFromGlobal(
		std::stod(value.at(0)),
		std::stod(value.at(1)),
		std::stod(value.at(2))
	);
}

/**
 * Old way:
 * <[tmnav_route_airport][Origin][]
 *                   <[string8][Identifier][KMTH]>
 * <[tmnav_route_airport][Destination][]
 *                  <[string8][Identifier][KMTH]>
 * New way:
 * <[tmnav_route_origin][EHAM][0]
 *                      <[string8][Identifier][EHAM]>
 * <[tmnav_route_destination][EHRD][4]
 *                      <[string8][Identifier][EHRD]>
 **/
std::tuple<std::string, std::string> AeroflyConfigFile::getFlightplan()
{
	std::string origin = "";
	origin = this->getValue(this->fileBuffer, "Identifier", "tmnav_route_origin\\]").c_str();
	if (origin.empty()) {
		origin = this->getValue(this->fileBuffer, "Identifier", "Origin").c_str();
	}

	std::string destination = "";
	destination = this->getValue(this->fileBuffer, "Identifier", "tmnav_route_destination\\]").c_str();
	if (destination.empty()) {
		destination = this->getValue(this->fileBuffer, "Identifier", "Destination").c_str();
	}

	return {
		origin,
		destination
	};
}

/**
 * @see https://www.aerofly.com/community/forum/index.php?thread/19105-custom-missions-converting-coordinates/
 */

AeroflyConfigFile::GeoCoordinates AeroflyConfigFile::LonLatFromGlobal(const double& x, const double& y, const double& z)
{
	const double f = 1.0 / 298.257223563; // WGS84
	const double e2 = 2 * f - f * f;

	const double lambda = this->VectorToAngle(x, y);
	const double rho = sqrt(x * x + y * y);

	double phi = atan(z / ((1.0 - e2) * rho));

	GeoCoordinates position;
	position.longitude = std::fmod(lambda * 180 / M_PI, 360.0);
	if (position.longitude > 180) {
		position.longitude -= 360;
	}
	position.latitude = phi * 180 / M_PI;

	return position;
}

// normal angle computation between two 3D-vectors
double AeroflyConfigFile::VectorToAngle(const double& x, const double& y)
{
	if (x > 0)
	{
		if (y < 0)
		{
			return 2 * M_PI + atan(y / x);
		}
		else
		{
			return atan(y / x);
		}
	}

	if (x < 0)
	{
		return M_PI + atan(y / x);
	}

	if (y > 0)
	{
		return 0.5 * M_PI;
	}

	return 1.5 * M_PI;
}

void AeroflyConfigFile::setFromAeroflyObject(const AeroflyWeather& aerofly)
{
	this->setDate(aerofly.year, aerofly.month, aerofly.day);
	this->setTime(aerofly.hours);
	this->setWind(aerofly.windStrength, aerofly.windDirection);
	this->setTurbulence(aerofly.windTurbulence);
	this->setThermalActivity(aerofly.thermalActivity);
	this->setPressure(aerofly.pressure);
	this->setVisibility(aerofly.visibility);

	for (int i = 0; i < 3; ++i) {
		this->setCloud(i, aerofly.clouds[i].height, aerofly.clouds[i].density);
	}
}

void AeroflyConfigFile::getToAeroflyObject(AeroflyWeather& aerofly)
{
	std::tie(aerofly.year, aerofly.month, aerofly.day) = this->getDate();
	aerofly.hours = this->getTime();
	std::tie(aerofly.windStrength, aerofly.windDirection) = this->getWind();
	aerofly.windTurbulence = this->getTurbulence();
	aerofly.thermalActivity = this->getThermalActivity();
	aerofly.pressure = this->getPressure();
	aerofly.visibility = this->getVisibility();

	for (int i = 0; i < 3; ++i) {
		std::tie(aerofly.clouds[i].height, aerofly.clouds[i].density) = this->getCloud(i);
	}
}
