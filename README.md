![](docs/favicon-64x64.png) Aerofly Wettergerät
================================================

Copy METAR weather information into [IPCAS' Aerofly FS 2](https://www.aerofly.com/).

The <i>Aerofly Wettergerät</i> will copy the following METAR weather information from remote sources to your configuration file while Aerofly FS 2 is _not_ running:

* Time and day (will set the year and month to current year and month because they are not present in METAR information)
* Wind & turbulences
* Thermal activity
* Clouds (height & density)

The <i>Aerofly Wettergerät</i> is capable of setting weather values which you cannot access in Aerofly FS 2. This is tested and is done deliberately.

> The <i>Aerofly Wettergerät</i> is the successor to ["Aerofly Weather" (AeroWX)](https://github.com/fboes/aerofly-weather/).

Requirements
------------

* Microsoft Windows 10 has to be installed. See below for other operating systems.
* IPACS Aerofly FS 2 has to be installed.
* A `main.mcf` has to be located at `%USERPROFILE%\Documents\Aerofly FS2\main.mcf`. If this is not the case point the tool to the file location by setting the `--file <FILE>` parameter.
* An internet connection to the [AVWX REST API](http://avwx.rest/) is required. If there is an internet connection but AVWX is not reachable, start the tool with the `--url <URL>` parameter set to a different METAR REST API.

Installation
------------

1. Download the latest release ZIP from https://github.com/fboes/aerofly-wettergeraet/releases/latest.
2. Unpack the ZIP file to some sensible location, e.g. `C:\Program Files\aerofly-wettergeraet`.
3. Create a shortcut on your desktop by right-clicking `aerofly-wettergeraet-desktop.exe` > "Send to" > "Desktop (Create shortcut)".  
   You might want to re-label the shortcut to "Aerofly Wettergerät".
3. Start the desktop application by clicking on the desktop link.

In case you want to use this application on a different operation system, consider [building your own executable from this project](CONTRIBUTING.md).

Usage
-----

This tool comes in two variants: The desktop application <i>Aerofly Wettergerät</i> (`aerofly-wettergeraet-desktop.exe`),…

![Aerofly Wettergerät Desktop Application](docs/aerofly-wx-desktop.png)

…and the command-line <i>Aerofly Wettergerät</i> (`aerofly-wettergeraet.exe`).

![Aerofly Wettergerät Command Line Application](docs/aerofly-wx-cli.png)

Both variants have more or less the same capabilities.

You will need a copy of IPACS' Aerofly FS 2 which has run at least once. This creates a file called `main.mcf`, which contains all settings and the weather data in Aerofly FS 2. This file will be modified by the application.

Before starting the <i>Aerofly Wettergerät</i> be sure that Aerofly FS 2 is _not_ running. On start-up the application will load the current settings from your `main.mcf`.

By starting the application METAR data will be fetched from a remote internet service for the given ICAO airport code. If it is successful, it will paste the METAR code and convert it into the corresponding Aerofly FS 2 values. It also allows for changing values which are not accessible from inside Aerofly FS 2.

Optionally you can enter a METAR code manually.

_Note:_ The desktop application requires you to actually hit the "Fetch" button to fetch METAR information from the internet, and to hit the "Save" button to copy the data to your `main.mcf`.

**Important:** Be sure to quit the application before starting Aerofly FS 2 to not accidentally change values while AFS2 is running and to free up memory. Also you may want to backup your `main.mcf` in case something goes wrong.

For troubleshooting look into the [Frequently Asked Questions](docs/faq.md).

HTTP services
-------------

This tool is compatible with the following METAR services:

* [AVWX](http://avwx.rest/): This is the default HTTP service.
* [CheckWX](https://www.checkwx.com/): You will need to get an API key to use this service.

You may also try any other HTTP service which offers METAR information as raw text or JSON. See the [configuration guide on how to set different METAR services](docs/configuration.md).

Update
------

Just repeat all steps for a regular installation.

Status
-------

![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/fboes/aerofly-wettergeraet.svg)
![GitHub](https://img.shields.io/github/license/fboes/aerofly-wettergeraet.svg)

Legal stuff
-----------

Author: [Frank Boës](https://3960.org)

Copyright & license: See [LICENSE.txt](LICENSE.txt)  
and [Curl's license](docs/LICENSE-curl.txt), also found at https://curl.haxx.se/docs/copyright.html  
and [WxWidgets's license](docs/LICENSE-wxwidgets.txt), also found at https://www.wxwidgets.org/about/licence/.

This tool is NOT affiliated with, endorsed, or sponsored by IPACS GbR. As stated in the [LICENSE.txt](LICENSE.txt), this tool comes with no warranty and might damage your files.

This software complies with the General Data Protection Regulation (GDPR) as it does not collect nor transmits any personal data but for data your submit by using the CLI commands using URLs. For GDPR compliance of these services refer to the legal statements of these services.

The <i>Aerofly Wettergerät</i> (German for "weather machine") is handcrafted <i>Wertarbeit</i>. 😉
