#include "pch.h"
#include "MetarVisibility.h"
#include "Convert.h"


MetarVisibility::MetarVisibility()
{
}

MetarVisibility::~MetarVisibility()
{
}

void MetarVisibility::setMeters(double meters)
{
	this->meters = meters;
	this->statuteMiles = Convert::metersToMiles(meters);
}

void MetarVisibility::setStatuteMiles(double statuteMiles)
{
	this->statuteMiles = statuteMiles;
	this->meters = Convert::milesToMeters(statuteMiles);
}

void MetarVisibility::setMax()
{
	this->setStatuteMiles(10);
}



