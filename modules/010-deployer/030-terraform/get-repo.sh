#!/usr/bin/env bash

source ../../functions.inc
#1.5.5-*

_apt_key_add \
  https://apt.releases.hashicorp.com/gpg \
  /usr/share/keyrings/hashicorp-archive-keyring.gpg

_apt_repo_add \
  "https://apt.releases.hashicorp.com $(_lsb_search VERSION_CODENAME) main" \
  "/etc/apt/sources.list.d/hashicorp.list" \
  "/usr/share/keyrings/hashicorp-archive-keyring.gpg"

# _apt_repo_update
