#include "stdafx.h"
#include "MetarParserSimple.h"
#include "Convert.h"
#include <string>
#include <sstream>
#include <iterator>
#include <regex>

void MetarParserSimple::initializeDate()
{
	if (this->observed.year != 1970) {
		return;
	}

	time_t rawtime;
	struct tm ptm;

	time(&rawtime);

	gmtime_s(&ptm, &rawtime);
	this->observed.year = ptm.tm_year + 1900;
	this->observed.month = ptm.tm_mon + 1;
	this->observed.day = ptm.tm_mday;
	this->observed.hours = ptm.tm_hour;
	this->observed.minutes = ptm.tm_min;
}

void MetarParserSimple::fixTimeDate()
{
	// This is a naive approach, but it does the job
	// Minutes
	while (observed.minutes >= 60) {
		observed.minutes -= 60;
		observed.hours++;
	}
	while (observed.minutes < 0) {
		observed.minutes += 60;
		observed.hours--;
	}

	// Hours
	while (observed.hours >= 24) {
		observed.hours -= 24;
		observed.day++;
	}
	while (observed.hours < 0) {
		observed.hours += 24;
		observed.day--;
	}

	// Day
	while (observed.day > 31) {
		observed.day -= 31;
		observed.month++;
	}
	while (observed.day < 1) {
		observed.day += 31;
		observed.month--;
	}

	// Month
	while (observed.month > 12) {
		observed.month -= 12;
		observed.year++;
	}
	while (observed.month < 1) {
		observed.month += 12;
		observed.year--;
	}
}

int MetarParserSimple::fixDegrees(int degrees)
{
	while (degrees >= 360) {
		degrees -= 360;
	}
	while (degrees < 0) {
		degrees += 360;
	}
	return degrees;
}

MetarParserSimple::MetarParserSimple(std::string metarString)
{
	// this->observed TODO
	if (metarString != "") {
		this->convert(metarString);
	}
}

MetarParserSimple::~MetarParserSimple()
{
}

bool MetarParserSimple::convert(std::string metarString)
{
	std::smatch match;

	if (metarString == "") {
		return false;
	}

	if (std::regex_search(metarString, match, std::regex(" (\\d) (\\d)/(\\d)(SM)"))) {
		// convert visbility range like `1 1/2 SM` to `3/2 SM`
		std::string newFract = std::to_string(
			std::stoi(match[1].str()) * std::stoi(match[3].str()) + std::stoi(match[2].str())
		);
		metarString = std::regex_replace(
			metarString,
			std::regex(" (\\d) (\\d)/(\\d)(SM)"),
			" " + newFract + "/" + match[3].str() + match[4].str()
		);
	}

	// start parsing
	// @see https://stackoverflow.com/questions/236129/how-do-i-iterate-over-the-words-of-a-string
	unsigned short parsingMode = 0;
	unsigned short currentCondition = 0;
	unsigned short sizeofConditions = sizeof(this->conditions) / sizeof(this->conditions[0]);
	this->numberOfClouds = 0;
	unsigned short sizeofClouds = sizeof(this->clouds) / sizeof(this->clouds[0]);
	std::string metarPart = "";
	std::stringstream metarParts(metarString);

	while (metarParts >> metarPart) {
		if (metarPart == "") {
			break;
		}
		if (parsingMode < 3 && std::regex_match(metarPart, std::regex("(\\d+)(?:/(\\d+))?(SM)?"))) {
			parsingMode = 3; // no wind reported
		}
		if (parsingMode < 5 && std::regex_match(metarPart, std::regex("(FEW|SCT|BKN|OVC)(\\d+).*"))) {
			parsingMode = 5; // no visibility / conditions reported
		}
		if (parsingMode < 6 && std::regex_match(metarPart, std::regex("M?\\d+/M?\\d+"))) {
			parsingMode = 6; // end of clouds
		}

		switch (parsingMode) {
		case 0:
			// ICAO Airport Code
			if (metarPart != "METAR" && std::regex_match(metarPart, match, std::regex("([A-Za-z0-9]+)"))) {
				strcpy_s(this->icao, 8, metarPart.c_str());
				parsingMode = 1;
			}
			// Non-standard date prepended, parsing it anyway
			else if (std::regex_match(metarPart, match, std::regex("([1-2]\\d{3})\\D([0-1]\\d)\\D([0-3]\\d)"))) {
				this->setDate2(
					std::stoi(match[1].str()),
					std::stoi(match[2].str()),
					std::stoi(match[3].str())
				);
			}
			break;
		case 1:
			// Observed Date
			if (std::regex_match(metarPart, match, std::regex("([0-3]\\d)([0-2]\\d)([0-5]\\d)Z"))) {
				this->setDate(
					std::stoi(match[1].str()),
					std::stoi(match[2].str()),
					std::stoi(match[3].str())
				);
				// Assume great visibility until proven otherwise
				this->visibilityMeters = Convert::milesToMeters(10);
				parsingMode = 2;
			}
			break;
		case 2:
			// Wind
			if (std::regex_match(metarPart, match, std::regex("(\\d\\d\\d|VRB)P?(\\d+)(?:G(\\d+))?(KT|MPS|KPH)"))) {
				if (match[1].str() == "VRB") {
					this->wind.degrees = 180;
					this->wind.degreesFrom = 0;
					this->wind.degreesTo = 359;
				}
				else {
					this->wind.degrees = this->fixDegrees(std::stoi(match[1].str()));
					this->wind.degreesFrom = this->fixDegrees(this->wind.degrees);
					this->wind.degreesTo = this->fixDegrees(this->wind.degrees);
				}

				std::string gust = (match[3].str() != "") ? match[3].str() : match[2].str();
				if (match[4].str() == "KT") {
					this->wind.speedKts = std::stoi(match[2].str());
					this->wind.gustKts = std::stoi(gust);
				}
				else if (match[4].str() == "MPS") {
					this->wind.speedKts = Convert::mpsToKts(std::stoi(match[2].str()));
					this->wind.gustKts = Convert::mpsToKts(std::stoi(gust));
				}
				else {
					this->wind.speedKts = Convert::kphToKts(std::stoi(match[2].str()));
					this->wind.gustKts = Convert::kphToKts(std::stoi(gust));
				}

				parsingMode = 3;
			}
			break;
		case 3:
			// Visibility
			if (std::regex_match(metarPart, match, std::regex("(\\d+)(?:/(\\d+))?(SM)?"))) {
				double visibility = std::stoi(match[1].str());
				if (match[2].str() != "") {
					// Fraction
					visibility /= std::stoi(match[2].str());
				}

				if (match[3].str() == "SM") {
					this->visibilityMeters = Convert::milesToMeters(visibility);
				}
				else {
					this->visibilityMeters = visibility;
				}

				parsingMode = 4;
			}
			else if (metarPart == "CAVOK" || metarPart == "CLR") {
				parsingMode = 5; // no clouds & conditions reported
			}
			else if (std::regex_match(metarPart, match, std::regex("(\\d+)V(\\d+)"))) {
				// Variable wind direction
				this->wind.degreesFrom = this->fixDegrees(std::stoi(match[1].str()));
				this->wind.degreesTo = this->fixDegrees(std::stoi(match[2].str()));
			}
			break;
		case 4:
			// Conditions
			if (std::regex_match(metarPart, match, std::regex("(\\+|-|VC|RE)?([A-Z][A-Z])([A-Z][A-Z])?([A-Z][A-Z])?"))) {
				for (unsigned int i = 1; i <= 4; i++) {
					if (currentCondition < sizeofConditions && match[i].str() != "") {
						strcpy_s(this->conditions[currentCondition], 4, match[i].str().c_str());
						currentCondition ++;
					}
				}
				// may occur multiple times
			}
			break;
		case 5:
			// Clouds
			if (this->numberOfClouds < sizeofClouds && std::regex_match(metarPart, match, std::regex("(FEW|SCT|BKN|OVC)(\\d+).*"))) {
				strcpy_s(this->clouds[this->numberOfClouds].code, 4, match[1].str().c_str());
				this->clouds[this->numberOfClouds].baseFeetAgl = (std::stoi(match[2].str()) * 100);
				if (strcmp(this->clouds[this->numberOfClouds].code, "FEW") == 0) {
					this->clouds[this->numberOfClouds].densityMinimum = 1;
					this->clouds[this->numberOfClouds].densityMaximum = 2;
				}
				else if (strcmp(this->clouds[this->numberOfClouds].code, "SCT") == 0) {
					this->clouds[this->numberOfClouds].densityMinimum = 3;
					this->clouds[this->numberOfClouds].densityMaximum = 4;
				}
				else if (strcmp(this->clouds[this->numberOfClouds].code, "BKN") == 0) {
					this->clouds[this->numberOfClouds].densityMinimum = 5;
					this->clouds[this->numberOfClouds].densityMaximum = 7;
				}
				else if (strcmp(this->clouds[this->numberOfClouds].code, "OVC") == 0) {
					this->clouds[this->numberOfClouds].densityMinimum = 8;
					this->clouds[this->numberOfClouds].densityMaximum = 8;
				}
				else {
					this->clouds[this->numberOfClouds].densityMinimum = 0;
					this->clouds[this->numberOfClouds].densityMaximum = 0;
				}
				this->ceiling = &this->clouds[this->numberOfClouds];
				this->numberOfClouds++;
			}
			break;
		case 6:
			// Temperature
			if (std::regex_match(metarPart, match, std::regex("(M)?(\\d+)/(M)?(\\d+)"))) {
				this->temperatureCelsius = std::stoi(match[2].str());
				if (match[1].str() == "M") {
					this->temperatureCelsius *= -1;
				}

				this->dewpointCelsius = std::stoi(match[4].str());
				if (match[3].str() == "M") {
					this->dewpointCelsius *= -1;
				}
				parsingMode = 7;
			}
			break;
		case 7:
			// Pressure
			if (std::regex_match(metarPart, match, std::regex("(Q|A)(\\d+)"))) {

				if ((match[1].str() == "Q")) {
					this->barometerKpa = std::stoi(match[2].str()) / 10.0;
				}
				else {
					this->barometerKpa = Convert::inhgToKpa(std::stoi(match[2].str()) / 100.0);

				}
				parsingMode = 8;
			}
			break;
		}
	}

	if (parsingMode < 2) {
		throw std::invalid_argument("Invalid METAR received");
		return false;
	}

	return true;
}

std::string MetarParserSimple::getFlightCategory()
{
	std::string flightCatgeory = "LIFR";
	double visibilityStatuteMiles = Convert::metersToMiles(this->visibilityMeters);

	if (visibilityStatuteMiles == 0) {
		return "";
	}
	else if (visibilityStatuteMiles > 5 && (this->ceiling == NULL || (*this->ceiling).baseFeetAgl > 3000)) {
		flightCatgeory = "VFR";
	}
	else if (visibilityStatuteMiles >= 3 && (this->ceiling == NULL || (*this->ceiling).baseFeetAgl >= 1000)) {
		flightCatgeory = "MVFR";
	}
	else if (visibilityStatuteMiles >= 1 && (this->ceiling == NULL || (*this->ceiling).baseFeetAgl >= 500)) {
		flightCatgeory = "IFR";
	}

	return flightCatgeory;
}

double MetarParserSimple::getHumidity()
{
	return exp(
		(17.625 * this->dewpointCelsius) / (243.04 + this->dewpointCelsius)
	) / exp(
	(17.625 * this->temperatureCelsius) / (243.04 + this->temperatureCelsius)
	);
}

void MetarParserSimple::setDate(short day, short hours, short minutes)
{
	this->initializeDate();

	// Day may be in preceeding month
	if (day > this->observed.day) {
		this->observed.month -= 1;
	}
	this->observed.day = day;
	this->observed.hours = hours;
	this->observed.minutes = minutes;
	this->fixTimeDate();
}

void MetarParserSimple::setDate2(int year, short month, short day)
{
	this->initializeDate();
	this->observed.year = year;
	this->observed.month = month;
	this->observed.day = day;
	this->fixTimeDate();
}

void MetarParserSimple::addHours(int hoursOffset)
{
	if (hoursOffset != 0) {
		this->observed.hours += hoursOffset, 23;
		this->fixTimeDate();
	}
}

bool MetarParserSimple::hasCondition(std::string testCondition)
{
	for (unsigned int i = 0; i <= sizeof(this->conditions) / sizeof(this->conditions[0]); i++) {
		if (strcmp(this->conditions[i], testCondition.c_str()) == 0) {
			return true;
		}
	}
	return false;
}
