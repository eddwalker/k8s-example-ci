#!/usr/bin/env bash

set -e

source ../../functions.inc

say "Install packages from ${PWD}/pkg/*.deb"
dpkg --skip-same-version --install pkg/*.deb

say "Create symlink"
ln -sf /usr/bin/tofu /usr/local/bin/terraform
