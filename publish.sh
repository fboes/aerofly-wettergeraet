#!/bin/bash
set -e

echo "Latest tag: "
git describe --tags
read -p 'Enter new semver string (e.g. "1.0.0"): ' VERSION

git merge master
git push
git checkout master
git merge develop
git tag -a "v${VERSION}" -m "${VERSION}"
git push && git push --tag
git checkout develop
start "https://github.com/fboes/aerofly-wettergeraet/releases"
start "https://flight-sim.org/filebase/index.php?file/439-aerofly-wetterger%C3%A4t/"
