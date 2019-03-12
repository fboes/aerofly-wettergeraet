![](./favicon-64x64.png) Configuration of Aerofly Weather 2 (AeroflyWX)
=======================================================================

For more information about command line options, call this tool with `--help` appended.

HTTP services
-------------

This tool is compatible with the following METAR services:

```batch
: Supply METAR manually
.\aerofly-wettergeraet.exe --metar ?

: AVWX - this is the default HTTP service
.\aerofly-wettergeraet.exe --url "http://avwx.rest/api/metar/XXXX?options=&format=json&onfail=cache"

: CheckWX - you will need to get an API key
.\aerofly-wettergeraet.exe --url "https://api.checkwx.com/metar/XXXX/decoded" --apikey "INSERT API KEY HERE"
```

Change default values
---------------------

You may set the default values for `.\aerofly-wettergeraet.exe` by setting environment variables:

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
