#!/usr/bin/env bash

source ../../functions.inc

say "Remove extra files"
rm -Rf /var/log/unattended-upgrades

say "Purge unused packages"
apt-get purge --yes \
    "*lxc*" \
    "liblxc*" \
    "lxd*" \
    snapd \
    unattended-upgrades \
    accountsservice \
    "libpolkit*" \
    2>&1 \
    | grep -vE '(Note, selecting|is not installed|service does not exist)'

say "apt-get autoremove"
apt-get autoremove --yes

# MEMO: Next lxc interfaces adding by cilium cni plugin, let lets keep it
# ip link \
#  | grep -oP "^\d+: \Klxc[^:@]+" \
#  | xargs -I% ip link del %


# MEMO: lvm was removed from purge since it may required for block plugins
