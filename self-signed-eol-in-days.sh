#!/usr/bin/env bash
# Check certificates expiration

# Manually handle errors
set +e

file=${1:-./self-signed.crt}
days=${2:-60}

chk="openssl x509 -noout -text -checkend $((days * 60 * 60 * 24))" # days to seconds
b="-----BEGIN CERTIFICATE-----"
e="-----END CERTIFICATE-----"
line=0
last=0
is_error=0

echo "INFO: Verify certificates expiration within $days days .."

while read -r i; do
  case $i in
    $b) printf -v cache -- "$i"
        line=$((line+1))
        last=$line
    ;;
    $e) printf -v cache -- "$cache\n$i"
        line=$((line+1))
        out="$($chk 2>&1 <<< "$cache")"
        case $? in
        	0) printf "\e[32m%s\e[0m %s:%s: " "OK  " "$file" "$last" ;;
        	*) printf "\e[31m%s\e[0m %s:%s: " "FAIL" "$file" "$last" ; is_error=1 ;;
        esac
        printf "$out" | grep -Ei '(Subject:|DNS:|Not after)' | xargs
    ;;
    *)  printf -v cache -- "$cache\n$i"
        line=$((line+1))
    ;;
  esac
done < $file

if [[ $is_error =~ ^1 ]]
then
  exit 1
fi
