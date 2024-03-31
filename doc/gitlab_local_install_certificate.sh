#!/usr/bin/env bash
# Run on you own host or server to append self signed certificates to ca store

set -e

host=gitlab.local
reg=gitlab.local:5005
temp=/tmp/${host}.crt

echo \
  | openssl s_client -servername $host -connect $host:443 2>/dev/null \
  | openssl x509 -text \
  | grep -A 999 '^-----BEGIN CERTIFICATE-----' \
  > $temp

if ! openssl x509 -in $temp > /dev/null ; then
  echo "Error: invalid certificate in file $temp:"
  cat $temp
  exit 1
fi

os_type="$(cat /etc/*-release || true)"

case "$os_type" in
  *debian*) sudo sh -c "echo Debian ca .. ; cp $temp /usr/local/share/ca-certificates/ && update-ca-certificates || exit 1" ;;
  *REDHAT*) sudo sh -c "echo Redhat ca .. ; cp $temp /etc/pki/ca-trust/source/anchors/ && update-ca-trust        || exit 1" ;;
  *)      echo "Error: unknown OS" && exit 1 ;;
esac

echo Docker /etc/docker/certs.d ..
certd=/etc/docker/certs.d/$reg
sudo mkdir -p $certd/
sudo cp $temp $certd/

rm $temp

echo OK
