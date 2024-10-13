# ![](./favicon-64x64.png) Frequently Asked Questions

## Why does the application crash on start-up?

Make sure to check these settings:

-   A `main.mcf` has to be located at `%USERPROFILE%\Documents\Aerofly FS 2\main.mcf`. If this is not the case point the tool to the file location by setting the `--file <FILE>` parameter, e.g. `aerofly-wettergeraet-desktop.exe --file "C:\…\main.mcf"`. For Aerofly FS 4 the file is located at `%USERPROFILE%\Documents\Aerofly FS 4\main.mcf`.
-   An internet connection to the [Aviation Weather Center REST API](https://aviationweather.gov/) is required. If there is an internet connection but the Aviation Weather Center REST API is not reachable, start the tool with the `--url <URL>` parameter set to a different METAR REST API, e.g. `aerofly-wettergeraet-desktop.exe --url "https://www.example.com/"`.

See [the <i>Wettergerät</i>'s configuration guide](configuration.md) on how to set parameters.

If these conditions are met but you are still experiencing crashes, please submit an [issue description](https://github.com/fboes/aerofly-wettergeraet/issues), we'll look into it.

## Can I save METAR data?

Sure you can. In the desktop application you will find "File > Save METAR file...", with a matching "File > Open METAR file" option. Saving METAR files will create `.rwx` files, which are regular `.txt` files with a fancy file suffix.

The RWX file format not only saves the METAR data, but also the exact date the METAR was requested - METAR data does not contain date information regarding year or month.

The CLI application is also able to read RWX files, but cannot write them. You are still free to save your favorite METAR string into a TXT file, as the <i>Aerofly Wettergerät</i> also reads TXT files.

## What does "Wettergerät" even mean?

Actually it is German. It means "weather device" and for German ears has a funny touch to it, as no one would call a thermometer or barometer a "Wettergerät".

"Wetter" is pronounced `\[ˈvɛtɐ ɡəˈʁɛːt\]`. Well, somewhat like "vetter" - "gay" - "rat", but without the "y" in "gay" and a long "a" in "rat".

Fun fact: a written German umlaut "ä" can be replaced with an "ae" - the two small dots above the "a" [are leftovers of an "e" placed above an "a"](<https://en.wikipedia.org/wiki/Diaeresis_(diacritic)#Umlaut>).

---

Return to [table of contents](README.md).
