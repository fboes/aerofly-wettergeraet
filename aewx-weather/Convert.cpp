#include "pch.h"
#include "Convert.h"

namespace Convert {
	double celsiusToFahrenheit(double celsius)
	{
		return celsius * 1.8 + 32;
	}

	double feetToMeters(double feet)
	{
		return feet * 0.3048;
	}

	double milesToMeters(double miles) {
		return miles * 1609.344;
	}

	double metersToMiles(double meters) {
		return meters / 1609.344;
	}

	double inhgToKpa(double inHg) {
		return inHg / 0.29529988;
	}

	double kpaToInhg(double kpa) {
		return kpa * 0.29529988;
	}

	double kphToMps(double kph) {
		return kph / 3600 * 1000;
	}

	double kphToKts(double kph)
	{
		return kph / 1.852;
	}

	double mpsToKts(double mps) {
		return mps * 1.9438445;
	}

	double ktsToMps(double kts) {
		return kts / 1.9438445;
	}
}
