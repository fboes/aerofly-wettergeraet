#pragma once
#include <string>

using namespace std;

/**
 * A collection of helper functions
 */
namespace BoShed
{
	// Limit value between min and max, by adding / substracting max
	int rotatingValue(int input, int max, int min = 0);

	// Limit value to 0..1
	double percent(double value, bool notMoreThan1 = false);

	// Output progress bar
	string progress(double value, std::string label = "", double valueBase = 100, std::string valueUnit = "%");
};

