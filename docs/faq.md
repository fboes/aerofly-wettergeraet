![](./favicon-64x64.png) Frequently Asked Questions
======================================

Why does the application crash on startup?
------------------------------------------

Make sure to check these settings:

* A `main.mcf` has to be located at `%USERPROFILE%\Documents\Aerofly FS2\main.mcf`. If this is not the case point the tool to the file location by setting the `--file <FILE>` parameter.
* An internet connection to the [AVWX REST API](http://avwx.rest/) is required. If there is an internet connection but AVWX is not reachable, start the tool with the `--url <URL>` parameter set to a different METAR REST API.

If these conditions are met but you are still experiencing crashes, please submit an [issue description](https://github.com/fboes/aerofly-wettergeraet/issues), we'll look into it.

---

Return to [table of contents](README.md).
