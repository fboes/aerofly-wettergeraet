![](./favicon-64x64.png) Configuration of the <i>Aerofly Wettergerät</i>
=======================================================================

For more information about command line options, call this tool with `--help` appended with the command-line variant of this tool.

HTTP services
-------------

This tool has been tested with multiple METAR services. To change the METAR service for the command line variant `aerofly-wettergeraet.exe`:

```batch
: Supply METAR manually
aerofly-wettergeraet.exe --metar ?

: AVWX - this is the default HTTP service
aerofly-wettergeraet.exe --url "http://avwx.rest/api/metar/XXXX?options=&format=json&onfail=cache"

: CheckWX - you will need to get an API key
aerofly-wettergeraet.exe --url "https://api.checkwx.com/metar/XXXX/decoded" --apikey "INSERT API KEY HERE"
```

To change the METAR service for the command line variant `aerofly-wettergeraet-desktop.exe` change the target of your desktop shortcut like this:

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

Return to [table of contents](../README.md).
