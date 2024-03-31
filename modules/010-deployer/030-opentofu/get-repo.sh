#!/usr/bin/env bash

set -e

source ../../functions.inc

_apt_key_add \
  https://get.opentofu.org/opentofu.gpg \
  /usr/share/keyrings/opentofu.gpg

_apt_key_add \
  https://packages.opentofu.org/opentofu/tofu/gpgkey \
  /usr/share/keyrings/opentofu-repo.gpg

_apt_repo_add \
  "https://packages.opentofu.org/opentofu/tofu/any/ any main" \
  "/etc/apt/sources.list.d/opentofu.list" \
  "/usr/share/keyrings/opentofu.gpg,/usr/share/keyrings/opentofu-repo.gpg"

# _apt_repo_update
