# Change log

## 1.7.5

-   Fixed parsing of cloud levels if there are more than 3 cloud levels
-   Improved time handling

## 1.7.4

-   💊 Fixing URL encoding issues

## 1.7.3

-   🎁 Improve menu structure, adding "Reset time & date"
-   💊 Limit date to current and past dates
-   💊 Updating dependencies

## 1.7.2

-   🎁 Keeping time intact while importing from `main.mcf` or METAR info
-   🎁 Added ICAO flight settings
-   🎁 Improved date parameter in URL

## 1.7.1

-   💊 Fixed time zone issues

## 1.7.0

-   🎁 Added support for [Aviation Weather Center](https://aviationweather.gov/) and date property
-   💊 Keep date / time at current date / time to enable fetching of current weather

## 1.6.1

-   💊 Fix VFR / IFR conditions, ignoring thin clouds

## 1.6.0

-   💊 Fix cloud layer height for Aerofly FS 4
-   🎁 World clock shows time at airport
-   🎁 Added pressure settings (not yet supported in "Aerofly FS 2" and "Aerofly FS 4")

## 1.5.2

-   💊 Improve position of cloud layers
-   🎁 Improve HTTP error messages
-   🎁 Show flight rules (VFR, MVFR, IFR, LIFR)

## 1.5.1

-   💊 Now also reads new MCF files

## 1.5.0

-   💊 Now also reads new MCF files

## 1.4.3

-   🎁 Improve error output while encountering API errors
-   💊 Switch default API to HTTPS

## 1.4.1

-   🎁 Improve error output while encountering API errors

## 1.4.0

-   🎁 Add extra header `Authorization` for API calls, improve documentation for API keys

## 1.3.1

-   🎁 Add airport names to labels, ignoring extra information while calling APIs
-   🎁 Make GUI sliders snap to next readable value
-   💊 Set maximum visibility to 15,000 meters, thus decreasing effects of low visibility

## 1.3.0

-   🎁 Switch GUI to feet and kts values

## 1.2.10

-

*   🎁 Append METAR information to clipboard file `clip.txt`
*   💊 Limit variable wind bearing impact relative to wind force

## 1.2.9

-   🎁 Add link to airport approach procedures and other useful information
-   🎁 Set default airport to flightplan _destination_
-   🎁 Add link to world clock

## 1.2.8

-   🎁 Switch ICAO airport code finder to https://opennav.com/search because of approach plates
-   🎁 Starting airport finder will open currently active airport code

## 1.2.7

-   💊 Fix error handling of HTTP requests

## 1.2.6

-   🎁 Open METAR file via CLI parameter
-   🎁 Add menu option to open, reload and save the Aerofly configuration file
-   🎁 Add accelerator keys

## 1.2.5

-   💊 Fix handling of `--file` parameter, as it did not work as advertised
-   💊 Improve desktop error output, restricting application to single instance
-   🎁 Open METAR file via CLI parameter (or double-click on desktop app)

## 1.2.4

-   🎁 Save METAR information to file

## 1.2.3

-   🎁 Add sample ICAO airport codes to combobox
-   🎁 Interpret date `YYYY/MM/DD` prepended to METAR string
-   🎁 Read METAR information from file

## 1.2.2

-   🎁 Add ICAO airport code combobox with codes from current flightplan

## 1.2.1

-   💊 Fix handling of `--file` parameter, as it did not work as advertised.
-   💊 Improve error handling
-   💊 Fix handling of non-valid JSON API replies.

## 1.2.0

-   💊 Remove unsecure functions
-   🎁 Add 64bit build settings

## 1.1.1

-   🎁 Add "Update" / "Help" menu entries
-   🎁 Add "Find ICAO airport codes" menu entries
-   🎁 Add marker for unsaved changes

## 1.1.0

-   💊 Fix METAR value translation into Aerofly Weather object (wind speed, visibility)

## 1.0.1

-   💊 Improve handling of ICAO airport codes
-   💊 Fix behaviour of seconds in UTC time

## 1.0.0

-   🎁 Initial release
