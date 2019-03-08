#include "pch.h"
#include "MetarParserSimple.h"
#include "Convert.h"
#include "BoShed.h"
#include <string>
#include <sstream>
#include <iterator>
#include <regex>

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
	unsigned short currentCloud = 0;
	std::string metarPart = "";
	std::stringstream metarParts(metarString);

	while (metarParts >> metarPart) {
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
			// ICAO Code
			if (metarPart != "METAR") {
				this->icao = metarPart;
				parsingMode = 1;
			}
			break;
		case 1:
			// Observed Date
			if (std::regex_match(metarPart, match, std::regex("(\\d\\d)(\\d\\d)(\\d\\d)Z"))) {
				this->setDate(
					std::stoi(match[1].str()),
					std::stoi(match[2].str()),
					std::stoi(match[3].str())
				);
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
				this->visibilityMeters = 9999;
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
				if (match[1].str() != "") {
					this->conditions.push_back(match[1].str());
				}
				if (match[2].str() != "") {
					this->conditions.push_back(match[2].str());
				}
				if (match[3].str() != "") {
					this->conditions.push_back(match[3].str());
				}
				if (match[4].str() != "") {
					this->conditions.push_back(match[4].str());
				}
				// may occur multiple times
			}
			break;
		case 5:
			// Clouds
			if (std::regex_match(metarPart, match, std::regex("(FEW|SCT|BKN|OVC)(\\d+).*")) && currentCloud < 3) {
				this->clouds[currentCloud].code = match[1].str();
				this->clouds[currentCloud].baseFeetAgl = (std::stoi(match[2].str()) * 100);
				if (this->clouds[currentCloud].code == "FEW") {
					this->clouds[currentCloud].densityMinimum = 1;
					this->clouds[currentCloud].densityMaximum = 2;
				}
				else if (this->clouds[currentCloud].code == "SCT") {
					this->clouds[currentCloud].densityMinimum = 3;
					this->clouds[currentCloud].densityMaximum = 4;
				}
				else if (this->clouds[currentCloud].code == "BKN") {
					this->clouds[currentCloud].densityMinimum = 5;
					this->clouds[currentCloud].densityMaximum = 7;
				}
				else if (this->clouds[currentCloud].code == "OVC") {
					this->clouds[currentCloud].densityMinimum = 8;
					this->clouds[currentCloud].densityMaximum = 8;
				}
				else {
					this->clouds[currentCloud].densityMinimum = 0;
					this->clouds[currentCloud].densityMaximum = 0;
				}
				this->ceiling = &this->clouds[currentCloud];
				currentCloud++;
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
	time_t rawtime;
	struct tm * ptm;

	time(&rawtime);

	ptm = gmtime(&rawtime);
	this->observed.year = ptm->tm_year + 1900;
	this->observed.month = ptm->tm_mon + 1;
	this->observed.day = (day >= 0) ? day : ptm->tm_mday;
	this->observed.hours = (hours >= 0) ? hours : ptm->tm_hour;
	this->observed.minutes = (minutes >= 0) ? minutes : ptm->tm_min;
	this->fixTimeDate();
}

void MetarParserSimple::addHours(int hoursOffset)
{
	if (hoursOffset != 0) {
		this->observed.hours += hoursOffset, 23;
		this->fixTimeDate();
	}
}
