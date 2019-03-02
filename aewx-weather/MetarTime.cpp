#include "pch.h"
#include "MetarTime.h"
#include "BoShed.h"
#include <time.h>

MetarTime::MetarTime(short day, short hours, short minutes)
{
	time_t rawtime;
	struct tm * ptm;

	time(&rawtime);

	ptm = gmtime(&rawtime);
	this->year = ptm->tm_year + 1900;
	this->month = ptm->tm_mon + 1;
	this->day = ptm->tm_mday;
	this->hours = ptm->tm_hour;
	this->minutes = ptm->tm_min;
	
	if (day >= 0) {
		this->setDay(day);
	}
	if (hours >= 0) {
		this->setHours(hours);
	}
	if (minutes >= 0) {
		this->setMinutes(hours);
	}
}

MetarTime::~MetarTime()
{
}


void MetarTime::setDay(unsigned int day) {
	this->day = BoShed::rotatingValue(day, 31, 1);
}

void MetarTime::setHours(unsigned int hours) {
	this->hours = BoShed::rotatingValue(hours, 23);
}

void MetarTime::addHours(int hours) {
	this->setHours(this->hours + hours);
}

void MetarTime::setMinutes(unsigned int minutes) {
	this->minutes = BoShed::rotatingValue(minutes, 59);
}
