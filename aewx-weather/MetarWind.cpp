#include "pch.h"
#include "MetarWind.h"
#include "BoShed.h"
#include "Convert.h"

MetarWind::MetarWind()
{
}

MetarWind::~MetarWind()
{
}

void MetarWind::setSpeedKts(double kts)
{
	this->speedKts = kts;
	this->speedMps = Convert::ktsToMps(kts);
}

void MetarWind::setSpeedMps(double mps)
{
	this->speedKts = Convert::mpsToKts(mps);
	this->speedMps = mps;
}

void MetarWind::setSpeedKph(double kph)
{
	this->setSpeedMps(Convert::kphToMps(kph));
}

void MetarWind::setGustKts(double kts)
{
	this->gustKts = kts;
	this->gustMps = Convert::ktsToMps(kts);
}

void MetarWind::setGustMps(double mps)
{
	this->gustKts = Convert::mpsToKts(mps);
	this->gustMps = mps;
}

void MetarWind::setGustKph(double kph)
{
	this->setGustMps(Convert::kphToMps(kph));
}

void MetarWind::setDegrees(unsigned int degrees)
{
	this->degrees = BoShed::rotatingValue(degrees, 359);
	if (this->degreesFrom == 0 && this->degreesTo == 0) {
		this->setDegreesRange(this->degrees, this->degrees);
	}
}

void MetarWind::setDegreesRange(unsigned int from, unsigned int to)
{
	this->degreesFrom = BoShed::rotatingValue(from, 359);
	this->degreesTo = BoShed::rotatingValue(to, 359);

}
