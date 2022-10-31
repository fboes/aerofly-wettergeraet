#!/bin/bash
set -e

echo "Latest tag: "
git describe --tags
read -p 'Enter new semver string (e.g. "1.0.0"): ' VERSION

git merge main
git push
git checkout main
git merge develop
git tag -a "v${VERSION}" -m "${VERSION}"
git push && git push --tag
git checkout develop
start "https://github.com/fboes/aerofly-wettergeraet/releases"
