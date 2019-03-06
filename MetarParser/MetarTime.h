#pragma once

/**
 * Get (current) UTC time pressed into some standard types.
 * As per convetion you should not change class variables, but use the corresponding
 * setters.
 */
class MetarTime
{
public:
	// Four-digit year
	int year;

	// 1..12
	int month;

	// 1..31
	int day;

	// 0..23
	int hours;

	// 0..59
	int minutes;

	MetarTime(short day = -1, short hours = -1, short minutes = -1);
	~MetarTime();

	void setDay(unsigned int day);

	void setHours(unsigned int hours);

	// Add hours (e.g. for timezone offset
	void addHours(int hours);

	void setMinutes(unsigned int minutes);
};

