#pragma once
class MetarTime
{
public:
	int year;    // Four-digit year
	int month;   // 1..12
	int day;     // 1..31
	int hours;   // 0..23
	int minutes; // 0..59

	MetarTime(short day = -1, short hours = -1, short minutes = -1);
	~MetarTime();

	void setDay(unsigned int day);
	void setHours(unsigned int hours);
	void addHours(int hours);
	void setMinutes(unsigned int minutes);
};

