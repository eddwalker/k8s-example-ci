#!/usr/bin/env bash

set -e

source ../../functions.inc

say "Remove symlink if exist"
rm -f /usr/local/bin/terraform

say "Install packages from ${PWD}/pkg/*.deb"
dpkg --skip-same-version --install pkg/*.deb

say "Rehash bash paths"
hash -r
