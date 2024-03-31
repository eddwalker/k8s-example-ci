#!/usr/bin/env bash
# This using to build deployer image

set -e

source ./modules/functions.inc

TIMEFORMAT='.. in %3R sec'

function _find_exec {
  for i in $(find modules/*/ -type f -name $1 | sort); do
    cd ${i%/*}
    printf "\e[32m%s\e[0m\n" "$i"
    time ./${i##*/}
    cd - > /dev/null
  done
}

# Setup env inside of builder helper
[[ -f /.env ]] && bash -x /.env && source /.env

# Set apt proxy if defind
if [[ -n $CI_PROXY_HTTP_APT ]] || [[ -n $CI_PROXY_HTTPS_APT ]]; then
  cat << EOF > /etc/apt/apt.conf.d/01proxy
  Acquire::HTTP::Proxy "$CI_PROXY_HTTP_APT";
  Acquire::HTTPS::Proxy "$CI_PROXY_HTTPS_APT";
EOF
fi

# Set http proxy if defined
[[ -n $CI_PROXY_HTTP_GET  ]] && export HTTP_PROXY="$CI_PROXY_HTTP_GET"
[[ -n $CI_PROXY_HTTPS_GET ]] && export HTTPS_PROXY="$CI_PROXY_HTTPS_GET"
[[ -n $CI_NOPROXY ]]         && export NO_PROXY="$CI_NOPROXY"

# Install buid time applications
time apt-get update

# TODO: time apt-get install --yes eatmydata
time apt-get install \
  --no-install-recommends \
  --no-install-suggests \
  --yes \
  ca-certificates \
  curl \
  file \
  gpg \
  bsdmainutils \
  wget \
  vim-tiny \
  git \
  jq \
  python3 python3-jsonnet python3-yaml

# Setup apt repos
_find_exec get-repo.sh

# Fetch added apt sources
time apt-get update

# Download packages
_find_exec get-pkg.sh

say "All done OK"
