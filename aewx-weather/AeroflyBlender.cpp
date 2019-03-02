#include "pch.h"
#include <stdlib.h>
#include "AeroflyBlender.h"

unsigned short AeroflyBlender::blendValue(double &currentValue, double targetValue, double step)
{
	if (currentValue == targetValue) {
		return 0;
	}

	if (currentValue < targetValue) {
		currentValue += step;
		if (currentValue > targetValue) {
			currentValue = targetValue;
		}
	}
	else {
		currentValue -= step;
		if (currentValue < targetValue) {
			currentValue = targetValue;
		}
	}
	return 1;
}

unsigned short AeroflyBlender::blendDegrees(unsigned int &currentValue, unsigned int targetValue, unsigned int step)
{
	if (currentValue == targetValue) {
		return 0;
	}

	// Give us some leeway
	int computedValue = currentValue;
	int range = currentValue - targetValue;

	if ((range < 0 && range > -180) || range > 180) {
		computedValue += ((int)step <= abs(range)) ? step : abs(range);
	}
	else {
		computedValue -= ((int)step <= abs(range)) ? step : abs(range);
	}

	// Fix over/underflow
	if (computedValue > 359) {
		computedValue -= 360;
	}
	else if (computedValue < 0) {
		computedValue += 360;
	}
	// Re-assign
	currentValue = computedValue;

	return 1;
}

AeroflyBlender::AeroflyBlender(AeroflyWeather *aerofly)
{
	this->targetSituation = aerofly;
	this->currentSituation = *aerofly;
}

AeroflyBlender::~AeroflyBlender()
{
}

unsigned short AeroflyBlender::blend(double step)
{
	unsigned short hasChanged = 0;
	hasChanged += this->blendValue((&this->currentSituation)->windStrength, this->targetSituation->windStrength, step);
	hasChanged += this->blendDegrees((&this->currentSituation)->windDirection, this->targetSituation->windDirection, (int)(step * 360));
	hasChanged += this->blendValue((&this->currentSituation)->windTurbulence, this->targetSituation->windTurbulence, step);
	hasChanged += this->blendValue((&this->currentSituation)->thermalActivity, this->targetSituation->thermalActivity, step);
	hasChanged += this->blendValue((&this->currentSituation)->visibility, this->targetSituation->visibility, step);

	for (int i = 0; i < 3; ++i) {
		hasChanged += this->blendValue((&this->currentSituation)->cloudHeight[i], this->targetSituation->cloudHeight[i], step);
		hasChanged += this->blendValue((&this->currentSituation)->cloudDensity[i], this->targetSituation->cloudDensity[i], step);
	}
	return hasChanged;
}

