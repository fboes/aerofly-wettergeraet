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

// If extraRange is activated, the minimumDensity is reduced by 1 to have some more range
double MetarCloud::getAverageDensity(bool extraRange) {
	auto minimum = this->densityMinimum;
	if (extraRange && minimum > 0) {
		minimum--;
	}
	return ((double)minimum + (double)this->densityMaximum) / 2.0;
}

// If extraRange is activated, the minimumDensity is reduced by 1 to have some more range
double MetarCloud::getRandomDensity(bool extraRange) {
	srand((int)time(NULL));
	auto minimum = this->densityMinimum;
	if (extraRange && minimum > 0) {
		minimum--;
	}

	return minimum + (
		(rand() % 100 / 100.0) * (this->densityMaximum - minimum)
	);
}

/* Returns a value 0..1. */
double MetarCloud::getAverageDensityPercent() {
	return this->convertEighthToPercent(this->getAverageDensity(true));
}

/* Returns a value 0..1. */
double MetarCloud::getRandomDensityPercent() {
	return this->convertEighthToPercent(this->getRandomDensity(true));
}

/* Returns a value 0..1. */
double MetarCloud::getMaximumDensityPercent() {
	return this->convertEighthToPercent(this->densityMaximum);
}

double MetarCloud::convertEighthToPercent(double eight) {
	return eight / (8.0);
}
