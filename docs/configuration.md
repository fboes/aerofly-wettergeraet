![](./favicon-64x64.png) Configuration of the <i>Aerofly Wettergerät</i>
=======================================================================

For more information about command line options, call the command line variant of this tool with `--help` appended to it. 

There is also a [source code documentation on command-line parameters](https://github.com/fboes/aerofly-wettergeraet/blob/master/src/WettergeraetLib/Argumentor.cpp#L20) applicable for the command-line as well as the desktop variant of this tool.

To append parameters to the desktop application, right-click your desktop icon, select "Properties" and append the parameter(s) to the shortcut's target.

![](modify-shortcut.png)

HTTP services
-------------

This tool has been tested with multiple METAR services. To change the METAR service for the command line variant `aerofly-wettergeraet.exe` append additional parameters:

```batch
: Supply METAR manually
aerofly-wettergeraet.exe --metar ?

: AVWX - this is the default HTTP service
aerofly-wettergeraet.exe --url "http://avwx.rest/api/metar/XXXX?options=&format=json&onfail=cache"

: CheckWX - you will need to get an API key
aerofly-wettergeraet.exe --url "https://api.checkwx.com/metar/XXXX/decoded" --apikey "INSERT API KEY HERE"
```

To change the METAR service for the desktop variant `aerofly-wettergeraet-desktop.exe` change the target of your desktop shortcut:

```batch
: AVWX - this is the default HTTP service
aerofly-wettergeraet-desktop.exe --url "http://avwx.rest/api/metar/XXXX?options=&format=json&onfail=cache"

: CheckWX - you will need to get an API key
aerofly-wettergeraet-desktop.exe --url "https://api.checkwx.com/metar/XXXX/decoded" --apikey "INSERT API KEY HERE"
```
Change default values
---------------------

You may set the default values for `aerofly-wettergeraet.exe` / `aerofly-wettergeraet-desktop.exe` by setting environment variables:

```batch
set  AEROFLYWX_URL="http://avwx.rest/api/metar/XXXX?options=&format=json&onfail=cache"
setx AEROFLYWX_URL %AEROFLYWX_URL%

set  AEROFLYWX_APIKEY="12345abcd"
setx AEROFLYWX_APIKEY %AEROFLYWX_APIKEY%

set  AEROFLYWX_RESPONSE="raw"
setx AEROFLYWX_RESPONSE %AEROFLYWX_RESPONSE%

set  AEROFLYWX_HOURS=-8
setx AEROFLYWX_HOURS %AEROFLYWX_HOURS%

set  AEROFLYWX_FILE="main.mcf"
setx AEROFLYWX_FILE %AEROFLYWX_FILE%
```

---

Return to [table of contents](README.md).
