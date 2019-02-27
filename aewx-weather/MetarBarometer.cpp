#include "pch.h"
#include "MetarBarometer.h"
#include "Convert.h"

MetarBarometer::MetarBarometer(double kpa)
{
	this->setKpa(kpa);
}

MetarBarometer::~MetarBarometer()
{
}

void MetarBarometer::setKpa(double kpa)
{
	this->inhg = Convert::kpaToInhg(kpa);
	this->kpa = kpa;
	this->mb = this->kpa * 10;
}

void MetarBarometer::setInhg(double inhg)
{
	this->inhg = inhg;
	this->kpa = Convert::inhgToKpa(inhg);
	this->mb = this->kpa * 10;
}
