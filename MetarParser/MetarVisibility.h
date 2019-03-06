#pragma once

/**
 * Representation of visibility range data. Will automatically do conversion into different units.
 * As per convetion you should not change class variables, but use the corresponding
 * setters.
 */
class MetarVisibility
{
public:
	double meters = 0;

	double statuteMiles = 0;

	MetarVisibility();
	~MetarVisibility();
	
	void setMeters(double meters);

	void setStatuteMiles(double statuteMiles);

	// Will set the visbility to the maximum range
	void setMax();
};

