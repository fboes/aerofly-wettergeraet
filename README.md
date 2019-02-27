Aerofly Weather 2 (AeroWX2)
===========================

Copy METAR weather information into [IPCAS' Aerofly FS 2](https://www.aerofly.com/).

The AeroWX tools will copy the following METAR weather information to your configuration file while Aerofly FS 2 is _not_ running:

* Time and day (will set the year and month to current year and month because they are not present in METAR information)
* Wind & turbulences
* Thermal activity
* Clouds (height & density)

The METAR weather information can be supplied manually, or can be fetched from an URL.

AeroWX is capable of setting weather values which you cannot access in Aerofly FS 2. This is tested and is done deliberately.

Build dependencies
------------------

* Curl `vcpkg install curl`

Legal stuff
-----------

Author: [Frank Boës](http://3960.org)

Copyright & license: See [LICENSE.txt](LICENSE.txt)
