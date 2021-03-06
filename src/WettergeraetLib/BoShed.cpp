#include "stdafx.h"
#include "BoShed.h"
#include <algorithm>

namespace BoShed {
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

	double rotatingValue(double input, const double max, const double min) {
		if (min > max) {
			throw std::invalid_argument("Min > max value error");
		}
		auto fullPull = max + 1.0 - min;
		while (input > max) {
			input -= fullPull;
		}
		while (input < min) {
			input += fullPull;
		}
		return input;
	}


	double percent(double value, bool notMoreThan1)
	{
		if (notMoreThan1 && value > 1.0) {
			return 1.0;
		}
		if (value < 0.0) {
			return 0.0;
		}
		return value;
	}

	std::string progress(double value, std::string label, double valueBase, std::string valueUnit)
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
