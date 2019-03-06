#include "pch.h"
#include "MetarParser.h"
#include <string>
#include <sstream>
#include <iterator>
#include <regex>

MetarParser::MetarParser(std::string metarString)
{
	if (metarString != "") {
		this->convert(metarString);
	}
}

MetarParser::~MetarParser()
{
}

bool MetarParser::convert(std::string metarString)
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
				this->observed.setDay(std::stoi(match[1].str()));
				this->observed.setHours(std::stoi(match[2].str()));
				this->observed.setMinutes(std::stoi(match[3].str()));
				parsingMode = 2;
			}
			break;
		case 2:
			// Wind
			if (std::regex_match(metarPart, match, std::regex("(\\d\\d\\d|VRB)P?(\\d+)(?:G(\\d+))?(KT|MPS|KPH)"))) {
				if (match[1].str() == "VRB") {
					this->wind.setDegrees(180);
					this->wind.setDegreesRange(0, 359);
				}
				else {
					this->wind.setDegrees(std::stoi(match[1].str()));
				}

				std::string gust = (match[3].str() != "") ? match[3].str() : match[2].str();
				if (match[4].str() == "KT") {
					this->wind.setSpeedKts(std::stoi(match[2].str()));
					this->wind.setGustKts(std::stoi(gust));
				}
				else if (match[4].str() == "MPS") {
					this->wind.setSpeedMps(std::stoi(match[2].str()));
					this->wind.setGustMps(std::stoi(gust));
				}
				else {
					this->wind.setSpeedKph(std::stoi(match[2].str()));
					this->wind.setGustKph(std::stoi(gust));
				}

				parsingMode = 3;
			}
			break;
		case 3:
			// Visibility
			if (std::regex_match(metarPart, match, std::regex("(\\d+)(?:/(\\d+))?(SM)?"))) {
				double speed = std::stoi(match[1].str());
				if (match[2].str() != "") {
					// Fraction
					speed /= std::stoi(match[2].str());
				}

				if (match[3].str() == "SM") {
					this->visibility.setStatuteMiles(speed);
				}
				else {
					this->visibility.setMeters(speed);
				}

				parsingMode = 4;
			}
			else if (metarPart == "CAVOK" || metarPart == "CLR") {
				this->visibility.setMax();
				parsingMode = 5; // no clouds & conditions reported
			}
			else if (std::regex_match(metarPart, match, std::regex("(\\d+)V(\\d+)"))) {
				// Variable wind direction
				this->wind.setDegreesRange(
					std::stoi(match[1].str()),
					std::stoi(match[2].str())
				);
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
				this->clouds[currentCloud].setCode(match[1].str());
				this->clouds[currentCloud].setFeetAgl(std::stoi(match[2].str()) * 100);
				this->ceiling = &this->clouds[currentCloud];
				currentCloud++;
			}
			break;
		case 6:
			// Temperature
			if (std::regex_match(metarPart, match, std::regex("(M)?(\\d+)/(M)?(\\d+)"))) {
				int temperature = std::stoi(match[2].str());
				if (match[1].str() == "M") {
					temperature *= -1;
				}
				this->temperature.setTemperature(temperature);

				int dewpoint = std::stoi(match[4].str());
				if (match[3].str() == "M") {
					dewpoint *= -1;
				}
				this->temperature.setDewpoint(dewpoint);
				parsingMode = 7;
			}
			break;
		case 7:
			// Pressure
			if (std::regex_match(metarPart, match, std::regex("(Q|A)(\\d+)"))) {

				if ((match[1].str() == "Q")) {
					this->barometer.setKpa(std::stoi(match[2].str()) / 10.0);
				}
				else {
					this->barometer.setInhg(std::stoi(match[2].str()) / 100.0);

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

std::string MetarParser::getFlightCategory()
{
	std::string flightCatgeory = "LIFR";

	if (this->visibility.statuteMiles == 0) {
		return "";
	}
	else if (this->visibility.statuteMiles > 5 && (this->ceiling == NULL || (*this->ceiling).baseFeetAgl > 3000)) {
		flightCatgeory = "VFR";
	}
	else if (this->visibility.statuteMiles >= 3 && (this->ceiling == NULL || (*this->ceiling).baseFeetAgl >= 1000)) {
		flightCatgeory = "MVFR";
	}
	else if (this->visibility.statuteMiles >= 1 && (this->ceiling == NULL || (*this->ceiling).baseFeetAgl >= 500)) {
		flightCatgeory = "IFR";
	}

	return flightCatgeory;
}
