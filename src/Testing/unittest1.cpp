#include "stdafx.h"
#include "CppUnitTest.h"
#include <string>
#include "MetarParserSimple.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Testing
{
	TEST_CLASS(MetarParserSimpleTest)
	{
	public:

		TEST_METHOD(TestAviationWeather1)
		{
			MetarParserSimple metar("KEYW 041053Z AUTO 13005KT 10SM CLR 24/22 A3000 RMK AO2 SLP159 T02440222");
			//Assert::AreEqual("KEYW", metar.icao);
			Assert::AreEqual(4, metar.observed.day);
			Assert::AreEqual(10, metar.observed.hours);
			Assert::AreEqual(53, metar.observed.minutes);
			Assert::AreEqual(130, (int)metar.wind.degrees);
			Assert::AreEqual(130, (int)metar.wind.degreesFrom);
			Assert::AreEqual(130, (int)metar.wind.degreesTo);
			Assert::AreEqual(5.0, metar.wind.speedKts);
			Assert::AreEqual(5.0, metar.wind.gustKts);
			Assert::IsTrue(metar.visibilityMeters > 16000);
			Assert::AreEqual(std::string("CLR"), metar.clouds[0].code);
			Assert::AreEqual(0, (int)metar.clouds[0].baseFeetAgl);
			Assert::AreEqual(0, (int)metar.clouds[0].densityMinimum);
			Assert::AreEqual(0, (int)metar.clouds[0].densityMaximum);
			Assert::AreEqual(std::string("CLR"), metar.clouds[1].code);
			Assert::AreEqual(0, (int)metar.clouds[1].baseFeetAgl);
			Assert::AreEqual(0, (int)metar.clouds[1].densityMinimum);
			Assert::AreEqual(0, (int)metar.clouds[1].densityMaximum);
			Assert::AreEqual(std::string("CLR"), metar.clouds[2].code);
			Assert::AreEqual(0, (int)metar.clouds[2].baseFeetAgl);
			Assert::AreEqual(0, (int)metar.clouds[2].densityMinimum);
			Assert::AreEqual(0, (int)metar.clouds[2].densityMaximum);
			Assert::AreEqual(24.0, metar.temperatureCelsius);
			Assert::AreEqual(22.0, metar.dewpointCelsius);
			Assert::IsTrue(metar.barometerKpa > 101);
			Assert::IsTrue(metar.barometerKpa < 102);
			Assert::IsTrue(metar.getHumidity() > 0.88);
			Assert::IsTrue(metar.getHumidity() < 0.89);
			Assert::AreEqual(std::string("VFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestWikipediaDe)
		{
			MetarParserSimple metar("EDDS 081620Z 29010KT 9999 FEW040TCU 09/M03 Q1012 NOSIG");
			//Assert::AreEqual("EDDS", metar.icao);
			Assert::AreEqual(8, metar.observed.day);
			Assert::AreEqual(16, metar.observed.hours);
			Assert::AreEqual(20, metar.observed.minutes);
			Assert::AreEqual(290, (int)metar.wind.degrees);
			Assert::AreEqual(290, (int)metar.wind.degreesFrom);
			Assert::AreEqual(290, (int)metar.wind.degreesTo);
			Assert::AreEqual(10.0, metar.wind.speedKts);
			Assert::AreEqual(10.0, metar.wind.gustKts);
			Assert::AreEqual(9999.0, metar.visibilityMeters);
			Assert::AreEqual(std::string("FEW"), metar.clouds[0].code);
			Assert::AreEqual(4000, (int)metar.clouds[0].baseFeetAgl);
			Assert::AreEqual(1, (int)metar.clouds[0].densityMinimum);
			Assert::AreEqual(2, (int)metar.clouds[0].densityMaximum);
			Assert::AreEqual(std::string("CLR"), metar.clouds[1].code);
			Assert::AreEqual(0, (int)metar.clouds[1].baseFeetAgl);
			Assert::AreEqual(0, (int)metar.clouds[1].densityMinimum);
			Assert::AreEqual(0, (int)metar.clouds[1].densityMaximum);
			Assert::AreEqual(std::string("CLR"), metar.clouds[2].code);
			Assert::AreEqual(0, (int)metar.clouds[2].baseFeetAgl);
			Assert::AreEqual(0, (int)metar.clouds[2].densityMinimum);
			Assert::AreEqual(0, (int)metar.clouds[2].densityMaximum);
			Assert::AreEqual(9.0, metar.temperatureCelsius);
			Assert::AreEqual(-3.0, metar.dewpointCelsius);
			//Assert::AreEqual(0.0, metar.barometerKpa);
			Assert::IsTrue(metar.barometerKpa > 101.0);
			Assert::IsTrue(metar.barometerKpa < 101.5);
			//Assert::AreEqual(0.0, metar.getHumidity());
			Assert::IsTrue(metar.getHumidity() > 0.42);
			Assert::IsTrue(metar.getHumidity() < 0.43);
			Assert::AreEqual(std::string("VFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestCheckWx20190107)
		{
			MetarParserSimple metar("KSFO 070121Z 19023KT 1 1/2SM R28R/6000VP6000FT -RA BKN004 BKN013 OVC035 15/12 A2970 RMK AO2 T01500122 PNO $");
			//Assert::AreEqual("KSFO", metar.icao);
			Assert::AreEqual(7, metar.observed.day);
			Assert::AreEqual(1, metar.observed.hours);
			Assert::AreEqual(21, metar.observed.minutes);
			Assert::AreEqual(190, (int)metar.wind.degrees);
			Assert::AreEqual(190, (int)metar.wind.degreesFrom);
			Assert::AreEqual(190, (int)metar.wind.degreesTo);
			Assert::AreEqual(23.0, metar.wind.speedKts);
			Assert::AreEqual(23.0, metar.wind.gustKts);
			Assert::IsTrue(metar.visibilityMeters > 2000);
			Assert::IsTrue(metar.visibilityMeters < 2500);
			Assert::AreEqual(std::string("BKN"), metar.clouds[0].code);
			Assert::AreEqual(400, (int)metar.clouds[0].baseFeetAgl);
			Assert::AreEqual(5, (int)metar.clouds[0].densityMinimum);
			Assert::AreEqual(7, (int)metar.clouds[0].densityMaximum);
			Assert::AreEqual(std::string("BKN"), metar.clouds[1].code);
			Assert::AreEqual(1300, (int)metar.clouds[1].baseFeetAgl);
			Assert::AreEqual(5, (int)metar.clouds[1].densityMinimum);
			Assert::AreEqual(7, (int)metar.clouds[1].densityMaximum);
			Assert::AreEqual(std::string("OVC"), metar.clouds[2].code);
			Assert::AreEqual(3500, (int)metar.clouds[2].baseFeetAgl);
			Assert::AreEqual(8, (int)metar.clouds[2].densityMinimum);
			Assert::AreEqual(8, (int)metar.clouds[2].densityMaximum);
			Assert::AreEqual(15.0, metar.temperatureCelsius);
			Assert::AreEqual(12.0, metar.dewpointCelsius);
			//Assert::AreEqual(0.0, metar.barometerKpa);
			Assert::IsTrue(metar.barometerKpa > 100);
			Assert::IsTrue(metar.barometerKpa < 101);
			//Assert::AreEqual(0.0, metar.getHumidity());
			Assert::IsTrue(metar.getHumidity() > 0.82);
			Assert::IsTrue(metar.getHumidity() < 0.83);
			Assert::AreEqual(std::string("IFR"), metar.getFlightCategory());
		}


	};
}
