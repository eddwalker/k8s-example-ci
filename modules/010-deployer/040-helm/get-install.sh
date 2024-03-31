#!/usr/bin/env bash

set -e

source ../../functions.inc

dpkg --skip-same-version --install pkg/*.deb
