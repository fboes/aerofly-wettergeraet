Aerofly Weather 2 (AeroflyWX)
==============================

Copy METAR weather information into [IPCAS' Aerofly FS 2](https://www.aerofly.com/).

AeroflyWX will copy the following METAR weather information to your configuration file while Aerofly FS 2 is _not_ running:

* Time and day (will set the year and month to current year and month because they are not present in METAR information)
* Wind & turbulences
* Thermal activity
* Clouds (height & density)

The METAR weather information can be supplied manually, or can be fetched from an URL.

AeroflyWX is capable of setting weather values which you cannot access in Aerofly FS 2. This is tested and is done deliberately.

HTTP services
-------------

This tool is compatible with the following METAR API services:

* http://avwx.rest/api/metar/XXXX?options=&format=json&onfail=cache (default service)
* https://3960.org/metar/ (set `--response raw`)
* https://api.checkwx.com/metar/XXXX/decoded (you will need an API key, set it via `--apikey <YOUAPIKEY>`)

Legal stuff
-----------

Author: [Frank Boës](http://3960.org)

Copyright & license: See [LICENSE.txt](LICENSE.txt) and [Curl's license](https://curl.haxx.se/docs/copyright.html)
