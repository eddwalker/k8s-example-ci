#!/usr/bin/env bash

source ../../functions.inc

_apt_key_add \
  "https://baltocdn.com/helm/signing.asc" \
  "/usr/share/keyrings/helm.gpg"

_apt_repo_add \
  "https://baltocdn.com/helm/stable/debian/ all main" \
  "/etc/apt/sources.list.d/helm-stable-debian.list" \
  "/usr/share/keyrings/helm.gpg"

# _apt_repo_update \
#  "/etc/apt/sources.list.d/helm-stable-debian.list"
