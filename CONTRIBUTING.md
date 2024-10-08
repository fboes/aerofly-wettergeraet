# Contributing

## Submitting issues

1. Search in the [Github issues](https://github.com/fboes/aerofly-wettergeraet), if a similar issue has already been filed. If so, please contribute to this issue.
2. On [opening a new issue on Github](https://github.com/fboes/aerofly-wettergeraet), add a descriptive title describing in a few words your problem. Think of the terms you would have searched for to find this issue.
3. Leave a reproducible list of steps leading to the error.
4. If possible add the Node and NPM version you ware using as well as the OS.

## Building

This project was built with Microsoft Visual Studio 2019.

Dependencies:

1. [Curl](https://curl.haxx.se/): Install via `vcpkg install curl curl:x64-windows`
1. [WxWidgets](https://www.wxwidgets.org/): Install via `vcpkg install wxwidgets wxwidgets:x64-windows`

Remember to integrate via `vcpkg integrate install`.

### Icons

The `docs` folder contains the application icons.

1. The application icon was as SVG build in [Inkscape](https://inkscape.org/).
1. From there it is exported as PNG.
1. The PNG is converted into the application icons via https://icoconvert.com/.
1. This icon gets copied to the corresponding projects.

## Structure of this project

-   `FetchUrl` fetches METAR strings from remote services
-   `MetarParser` converts METAR strings into a structured object. Lots of `Metar*` objects assist in doing so.
-   `AeroflyWeather` converts METAR objects into values usable in Aerofly FS 2/4. `AeroflyBlender` can gradually change the current weather from current to target weather.
-   `AeroflyConfigFile` save Aerofly FS 2/4 values into the Aerofly FS 2/4 main config file `main.mcf`.

## Making changes

1. [Fork the repository on GitHub](https://help.github.com/articles/fork-a-repo/)
2. Load this project into your code editor.
3. Create a new feature branch from `develop` branch, like `feature/my-cool-feature`.
4. Start developing. Coding guidelines will be enforced by `.editorconfig`.
5. If you are really nice you will supply a test for the stuff you coded.
6. Add a line to the `CHANGELOG.md` to tell people what you did in your feature.
7. Commit with a meaningful commit message (e.g. the line you put into `CHANGELOG.md`) & push to your repository.
8. Run testing procedures. Be sure that all tests pass before proceeding any further.
9. [Submit a pull request](https://help.github.com/articles/about-pull-requests/).

### Emojis for `CHANGELOG.md`

-   🎁 `:gift:` New feature
-   💊 `:pill:` Bugfix, repairing a broken functionality
-   💣 `:bomb:` Possibly breaking change, needs further explanation
-   🔧 `:wrench:` Internal, technical improvement
