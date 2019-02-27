#pragma once
class MetarVisibility
{
public:
	double meters = 0;
	double statuteMiles = 0;

	MetarVisibility();
	~MetarVisibility();
	
	void setMeters(double meters);
	void setStatuteMiles(double statuteMiles);
	void setMax();
};

