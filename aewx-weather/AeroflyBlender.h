#pragma once
#include "AeroflyWeather.h"

/**
 * Blends this->currentSituation into this->targetSituation, gradually changing the weather.
 */
class AeroflyBlender
{
private:
	// The weather as it should be
	AeroflyWeather *targetSituation;

	// Return 1 if currentValue was changed, 0 if not
	unsigned short blendValue(double &currentValue, double targetValue, double step = 0.01);

	// Return 1 if currentValue was changed, 0 if not
	unsigned short blendDegrees(unsigned int &currentValue, unsigned int targetValue, unsigned int step = 1);

public:
	// The weather as it currently is
	AeroflyWeather currentSituation;

	AeroflyBlender(AeroflyWeather *aerofly);
	~AeroflyBlender();

	// Blend this->currentSituation into target->situation.
	// Step changes it percentual, e.g. `0.01` changes it by 1% per call.
	// Returns number of changed values.
	unsigned short blend(double step = 0.01);
};

