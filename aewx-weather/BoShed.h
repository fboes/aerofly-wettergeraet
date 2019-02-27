#pragma once
#include <string>

using namespace std;

namespace BoShed
{
	int rotatingValue(int input, int max, int min = 0);

	double percent(double value, bool notMoreThan1 = false);

	string progress(double value, std::string label = "", double valueBase = 100, std::string valueUnit = "%");
};

