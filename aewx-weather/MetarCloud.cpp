#include "pch.h"
#include "MetarCloud.h"
#include "Convert.h"
#include <string>
#include <time.h>

MetarCloud::MetarCloud(std::string code, unsigned long baseFeetAgl) {
	this->setCode(code);
	this->setFeetAgl(baseFeetAgl);
}

MetarCloud::~MetarCloud()
{
}

void MetarCloud::setCode(std::string code) {
	this->code = code;
	if (code == "FEW") {
		this->densityMinimum = 1;
		this->densityMaximum = 2;
	}
	else if (code == "SCT") {
		this->densityMinimum = 3;
		this->densityMaximum = 4;
	}
	else if (code == "BKN") {
		this->densityMinimum = 5;
		this->densityMaximum = 7;
	}
	else if (code == "OVC") {
		this->densityMinimum = 8;
		this->densityMaximum = 8;
	}
	else {
		this->densityMinimum = 0;
		this->densityMaximum = 0;
	}
}

void MetarCloud::setFeetAgl(unsigned long baseFeetAgl) {
	this->baseFeetAgl = baseFeetAgl;
	this->baseMetersAgl = (int)Convert::feetToMeters(baseFeetAgl);
}

double MetarCloud::getAverageDensity(unsigned short extraMax) {
	return (this->densityMinimum + this->densityMaximum + extraMax) / 2.0;
}

double MetarCloud::getRandomDensity(unsigned short extraMax) {
	srand((int)time(NULL));
	return this->densityMinimum + (
		(rand() % 100 / 100.0) * (this->densityMaximum + extraMax - this->densityMinimum)
		);
}

/* Returns a value 0..1. For this method this->densityMaximum is incremented by 1. */
double MetarCloud::getAverageDensityPercent() {
	return this->convertEighthToPercent(this->getAverageDensity(1), 1);
}

/* Returns a value 0..1. For this method this->densityMaximum is incremented by 1. */
double MetarCloud::getRandomDensityPercent() {
	return this->convertEighthToPercent(this->getRandomDensity(1), 1);
}

/* Returns a value 0..1. For this method this->densityMaximum is incremented by 1. */
double MetarCloud::getMaximumDensityPercent() {
	return this->convertEighthToPercent(this->densityMaximum + 1.0, 1);
}

double MetarCloud::convertEighthToPercent(double eight, unsigned short extraMax) {
	return eight / (8.0 + extraMax);
}