# ![](./favicon-64x64.png) More information on METAR

Wikipedia has an [article on METAR information](https://en.wikipedia.org/wiki/METAR) explaining the very basics.

## Sources for METAR information

These sites make METAR information publicly available:

-   https://en.allmetsat.com/metar-taf/
-   https://aviationweather.gov/metar
-   https://metars.com/

Copy METAR strings found on these pages to `aerowx-metar`.

## Some METAR examples

Some clouds over San Francisco (USA):

```
KSFO 050756Z 11007KT 10SM SCT035 BKN200 10/06 A2983 RMK AO2 SLP099 T01000061 401720039
```

Rainy in New York (USA) with limited visibility:

```
KLGA 050808Z 04007KT 1SM R04/P6000FT -RA BR BKN006 OVC014 06/04 A2967 RMK AO2 P0001 T00560044
```

A foggy night night in Miami (USA):

```
KMIA 050753Z 18007KT 5SM BR FEW005 23/23 A2997 RMK AO2 SLP149 T02280228
```

No wind in Geneva (Switzerland):

```
LSGG 050820Z VRB01KT 9999 FEW070 BKN110 M03/M04 Q1033 NOSIG
```

## Use CheckWX API

If your want to use <i>Aerofly Wettergerät</i> with CheckWx you will need to apply for a CheckWX API key. See https://api.checkwx.com/ on how to get an API key.

After receiving your API key, you will need to enter this key into your desktop batch file or your [local command line tools](command-line.md).

---

Return to [table of contents](README.md).
