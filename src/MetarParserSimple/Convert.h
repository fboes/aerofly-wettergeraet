#pragma once

/**
 * Convert between units, like from feet to meters.
 */
namespace Convert
{
	double celsiusToFahrenheit(double celsius);

	double feetToMeters(double feet);

	// -> statute miles
	double milesToMeters(double miles);

	// -> statute miles
	double metersToMiles(double meters);

	double inhgToKpa(double inHg);

	double kpaToInhg(double kpa);

	double kphToMps(double kph);

	// -> nautical miles
	double kphToKts(double kph);

	// -> nautical miles
	double mpsToKts(double mps);

	// -> nautical miles
	double ktsToMps(double kts);
};
