#pragma once
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
	void setDegrees(unsigned int degrees);
	void setDegreesRange(unsigned int from, unsigned int to);
};

