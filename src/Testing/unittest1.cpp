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
			Assert::IsTrue(strcmp(metar.icao, "KEYW") == 0);
			Assert::AreEqual(4, metar.observed.day);
			Assert::AreEqual(10, metar.observed.hours);
			Assert::AreEqual(53, metar.observed.minutes);
			Assert::AreEqual(130, (int)metar.wind.degrees);
			Assert::AreEqual(130, (int)metar.wind.degreesFrom);
			Assert::AreEqual(130, (int)metar.wind.degreesTo);
			Assert::AreEqual(5.0, metar.wind.speedKts);
			Assert::AreEqual(5.0, metar.wind.gustKts);
			Assert::IsTrue(metar.visibilityMeters > 16000);
			Assert::AreEqual("CLR", metar.clouds[0].code);
			Assert::AreEqual(0, (int)metar.clouds[0].baseFeetAgl);
			Assert::AreEqual(0, (int)metar.clouds[0].densityMinimum);
			Assert::AreEqual(0, (int)metar.clouds[0].densityMaximum);
			Assert::AreEqual("CLR", metar.clouds[1].code);
			Assert::AreEqual(0, (int)metar.clouds[1].baseFeetAgl);
			Assert::AreEqual(0, (int)metar.clouds[1].densityMinimum);
			Assert::AreEqual(0, (int)metar.clouds[1].densityMaximum);
			Assert::AreEqual("CLR", metar.clouds[2].code);
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

		TEST_METHOD(TestAviationWeather2)
		{
			MetarParserSimple metar("KACV 041053Z AUTO 07003KT 10SM CLR 04/04 A3001 RMK AO2 SLP169 T00440039");
			Assert::IsTrue(strcmp(metar.icao, "KACV") == 0);
			Assert::AreEqual("CLR", metar.clouds[0].code);
			Assert::AreEqual(std::string("VFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestAviationWeather3)
		{
			MetarParserSimple metar("KEYW 050653Z AUTO 19006KT FEW024 BKN039 26/23 A3000 RMK AO2 LTG DSNT W SLP159 T02610228");
			Assert::IsTrue(strcmp(metar.icao, "KEYW") == 0);
			Assert::AreEqual(190, (int)metar.wind.degrees);
			Assert::IsTrue(metar.visibilityMeters > 16000);
			Assert::AreEqual("FEW", metar.clouds[0].code);
			Assert::AreEqual("BKN", metar.clouds[1].code);
			Assert::AreEqual("CLR", metar.clouds[2].code);
			Assert::AreEqual(26.0, metar.temperatureCelsius);
			Assert::AreEqual(23.0, metar.dewpointCelsius);
			Assert::AreEqual(std::string("VFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestCheckWx1)
		{
			MetarParserSimple metar("KPIE 260853Z AUTO 02013G17KT 10SM CLR 17/07 A2998 RMK AO2 SLP153 T01720072 57000");
			Assert::IsTrue(strcmp(metar.icao, "KPIE") == 0);
			Assert::AreEqual("CLR", metar.clouds[0].code);
			Assert::AreEqual(std::string("VFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestCheckWx2)
		{
			MetarParserSimple metar("KSPG 260853Z AUTO 05012KT 10SM CLR 18/09 A2997 RMK AO2 SLP148 T01830094 53001");
			Assert::IsTrue(strcmp(metar.icao, "KSPG") == 0);
			Assert::AreEqual("CLR", metar.clouds[0].code);
			Assert::AreEqual(std::string("VFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestWikipediaDe)
		{
			MetarParserSimple metar("EDDS 081620Z 29010KT 9999 FEW040TCU 09/M03 Q1012 NOSIG");
			Assert::IsTrue(strcmp(metar.icao, "EDDS") == 0);
			Assert::AreEqual(8, metar.observed.day);
			Assert::AreEqual(16, metar.observed.hours);
			Assert::AreEqual(20, metar.observed.minutes);
			Assert::AreEqual(290, (int)metar.wind.degrees);
			Assert::AreEqual(290, (int)metar.wind.degreesFrom);
			Assert::AreEqual(290, (int)metar.wind.degreesTo);
			Assert::AreEqual(10.0, metar.wind.speedKts);
			Assert::AreEqual(10.0, metar.wind.gustKts);
			Assert::AreEqual(9999.0, metar.visibilityMeters);
			Assert::AreEqual("FEW", metar.clouds[0].code);
			Assert::AreEqual(4000, (int)metar.clouds[0].baseFeetAgl);
			Assert::AreEqual(1, (int)metar.clouds[0].densityMinimum);
			Assert::AreEqual(2, (int)metar.clouds[0].densityMaximum);
			Assert::AreEqual("CLR", metar.clouds[1].code);
			Assert::AreEqual(0, (int)metar.clouds[1].baseFeetAgl);
			Assert::AreEqual(0, (int)metar.clouds[1].densityMinimum);
			Assert::AreEqual(0, (int)metar.clouds[1].densityMaximum);
			Assert::AreEqual("CLR", metar.clouds[2].code);
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

		TEST_METHOD(TestWikipediaEn1)
		{
			MetarParserSimple metar("METAR LBBG 041600Z 12012MPS 090V150 1400 R04/P1500N R22/P1500U +SN BKN022 OVC050 M04/M07 Q1020 NOSIG 8849//91=");
			Assert::IsTrue(strcmp(metar.icao, "LBBG") == 0);
			// Test m/s
			// Test variable wind
			Assert::AreEqual("BKN", metar.clouds[0].code);
			Assert::AreEqual("OVC", metar.clouds[1].code);
			Assert::AreEqual("CLR", metar.clouds[2].code);
			Assert::AreEqual(std::string("LIFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestWikipediaEn2)
		{
			MetarParserSimple metar("METAR KTTN 051853Z 04011KT 1/2SM VCTS SN FZFG BKN003 OVC010 M02/M02 A3006 RMK AO2 TSB40 SLP176 P0002 T10171017=");
			Assert::IsTrue(strcmp(metar.icao, "KTTN") == 0);
			Assert::AreEqual("BKN", metar.clouds[0].code);
			Assert::AreEqual("OVC", metar.clouds[1].code);
			Assert::AreEqual("CLR", metar.clouds[2].code);
			// Test kpa
			Assert::AreEqual(std::string("LIFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestAllmetsatTaf1)
		{
			MetarParserSimple metar("KEYW 041053Z AUTO 13005KT 10SM CLR 24/22 A3000 RMK AO2 SLP159 T02440222");
			Assert::IsTrue(strcmp(metar.icao, "KEYW") == 0);
			Assert::AreEqual("CLR", metar.clouds[0].code);
			Assert::AreEqual(std::string("VFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestAllmetsatTaf2)
		{
			MetarParserSimple metar("EDDH 041050Z 29013KT 6000 SCT006 BKN009 04/03 Q1028 TEMPO BKN012");
			Assert::IsTrue(strcmp(metar.icao, "EDDH") == 0);
			Assert::AreEqual("SCT", metar.clouds[0].code);
			Assert::AreEqual("BKN", metar.clouds[1].code);
			Assert::AreEqual(std::string("IFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestAllmetsatTaf3)
		{
			MetarParserSimple metar("ETEB 041056Z AUTO 26010KT 9999 SCT090 00/M01 A3052 RMK AO2 SLP378 T10031013");
			Assert::IsTrue(strcmp(metar.icao, "ETEB") == 0);
			Assert::AreEqual("SCT", metar.clouds[0].code);
			Assert::AreEqual("CLR", metar.clouds[1].code);
			Assert::AreEqual(std::string("VFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestCheckWx20190107)
		{
			MetarParserSimple metar("KSFO 070121Z 19023KT 1 1/2SM R28R/6000VP6000FT -RA BKN004 BKN013 OVC035 15/12 A2970 RMK AO2 T01500122 PNO $");
			Assert::IsTrue(strcmp(metar.icao, "KSFO") == 0);
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
			Assert::AreEqual("BKN", metar.clouds[0].code);
			Assert::AreEqual(400, (int)metar.clouds[0].baseFeetAgl);
			Assert::AreEqual(5, (int)metar.clouds[0].densityMinimum);
			Assert::AreEqual(7, (int)metar.clouds[0].densityMaximum);
			Assert::AreEqual("BKN", metar.clouds[1].code);
			Assert::AreEqual(1300, (int)metar.clouds[1].baseFeetAgl);
			Assert::AreEqual(5, (int)metar.clouds[1].densityMinimum);
			Assert::AreEqual(7, (int)metar.clouds[1].densityMaximum);
			Assert::AreEqual("OVC", metar.clouds[2].code);
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

		TEST_METHOD(TestEhamWithCavok)
		{
			MetarParserSimple metar("EHAM 100125Z 33004KT CAVOK M00/M01 Q1026 NOSIG");
			Assert::IsTrue(strcmp(metar.icao, "EHAM") == 0);
			Assert::AreEqual("CLR", metar.clouds[0].code);
			Assert::AreEqual("CLR", metar.clouds[1].code);
			Assert::AreEqual(std::string("VFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestWithoutVis)
		{
			MetarParserSimple metar("KEYW 291553Z VRB03KT CLR 17/09 A3009 RMK AO2 SLP189 T01670089 $");
			Assert::IsTrue(strcmp(metar.icao, "KEYW") == 0);
			Assert::AreEqual(180, (int)metar.wind.degrees);
			Assert::IsTrue(metar.visibilityMeters > 16000);
			Assert::AreEqual("CLR", metar.clouds[0].code);
			Assert::AreEqual("CLR", metar.clouds[1].code);
			Assert::AreEqual(17.0, metar.temperatureCelsius);
			Assert::AreEqual(9.0, metar.dewpointCelsius);
			Assert::AreEqual(std::string("VFR"), metar.getFlightCategory());
		}

		TEST_METHOD(TestAutoDoesStuff)
		{
			MetarParserSimple metar("KDVO 022335Z AUTO 4SM BR BKN007 BKN013 12/12 A2988 RMK AO2");
			Assert::IsTrue(strcmp(metar.icao, "KDVO") == 0);
			Assert::AreEqual("BKN", metar.clouds[0].code);
			Assert::AreEqual("BKN", metar.clouds[1].code);
			Assert::IsTrue(metar.visibilityMeters > 6000);
			Assert::IsTrue(metar.visibilityMeters < 6500);
		}
	};
}
