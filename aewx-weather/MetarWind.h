#pragma once

/**
 * Representation of winddata. Will automatically do conversion into different units.
 * As per convetion you should not change class variables, but use the corresponding
 * setters.
 */
class MetarWind
{
public:
	unsigned int degrees = 0;     // 0..359
	unsigned int degreesFrom = 0; // 0..359
	unsigned int degreesTo = 0;   // 0..359
	double speedKts = 0;
	double speedMps = 0;
	double gustKts = 0;
	double gustMps = 0;

	MetarWind();
	~MetarWind();

	void setSpeedKts(double kts);

	void setSpeedMps(double mps);

	void setSpeedKph(double kph);

	void setGustKts(double kts);

	void setGustMps(double mps);

	void setGustKph(double kph);

	// Set wind directions 0..359
	void setDegrees(unsigned int degrees);

	// Use this if the wind is coming from different directions
	void setDegreesRange(unsigned int from, unsigned int to);
};

