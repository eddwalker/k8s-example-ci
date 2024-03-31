#!/usr/bin/env bash

set -e

source ../../functions.inc

# FIXME: below a kubectl hack to avoid build packages twice

dir="$(grep -R -l -m1 ^kubectl= ../../*/*/packages.txt)"
dir="${dir%/*}"

_var_validate \
	dir

cd $dir/pkg

dpkg --skip-same-version --force depends --install \
  cri-tools_*.deb \
  kubectl_*.deb

mkdir -p /etc/bash_completion.d
for i in crictl kubectl
do
  $i completion bash > /etc/bash_completion.d/$i
done

cd - > /dev/null
