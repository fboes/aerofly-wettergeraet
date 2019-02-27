#include "pch.h"
#include "BoShed.h"
#include <string>
#include <algorithm>

using namespace std;

namespace BoShed {
	/* Limit value between min and max, by adding / substracting max */
	int rotatingValue(int input, const int max, const int min) {
		if (min > max) {
			throw std::invalid_argument("Min > max value error");
		}
		auto fullPull = max + 1 - min;
		while (input > max) {
			input -= fullPull;
		}
		while (input < min) {
			input += fullPull;
		}
		return input;
	}

	/* Limit value to 0..1 */
	double percent(double value, bool notMoreThan1)
	{
		if (notMoreThan1 && value > 1.0) {
			return value;
		}
		if (value < 0.0) {
			return 0.0;
		}
		return value;
	}

	/* Out progress bar */
	string progress(double value, std::string label, double valueBase, std::string valueUnit)
	{
		const int calcValue = (int)std::round(std::min(100.0, std::max(0.0, value * 100)));
		label = label.substr(0, 10);
		const int base = 5;

		int fraction = calcValue % base;
		std::string string = " ";
		string += label + std::string(12 - label.length(), ' ');
		string += std::string((int)std::floor(calcValue / base), '=');
		string += fraction ? "~" : "";
		string += std::string((int)std::floor((100 - calcValue) / base), '-');
		string += "  " + std::to_string((int)std::round(value * valueBase)) + valueUnit;

		return string;
	}
}