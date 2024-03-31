
function say {
  printf "\e[4m%s\e[0m\n" "$@"
}

function cross {
  printf "\e[9m%s\e[0m\n" "$@"
}

function ita {
  printf "\e[3m%s\e[0m\n" "$@"
}

function bold  {
  printf "\e[1m%s\e[0m\n" "$@"
}

function info {
  printf "\e[032m%s\e[0m\n" "$@"
}

function warn {
  printf "\e[033m%s\e[0m\n" "$@"
}

function err {
  printf "\e[031mError: %s\e[0m\n" "$@"
  exit 1
}

function errnotexit {
  printf "\e[031m%s\e[0m\n" "$@"
}

function _apt_repo_update {
  say "Update list"
  if [[ -n "$@" ]]
  then
    for i in "$@"
    do
      # -o Dir::Etc::SourceList by default update all lists in dir with given list
      apt-get update -o Dir::Etc::SourceParts=/dev/null -o Dir::Etc::SourceList="$i"
    done
  else
    apt-get update
  fi
  echo "Update list exit code: $?"
}

function _apt_key_add {
  say  "Install key"
  echo "src: $1"
  echo "dst: $2"
  mkdir -p /usr/share/keyrings         || err "cant create dir"
  local tmp_dir="$(mktemp -d)"         || err "cant create tmp"
  local tmp_file=$tmp_dir/apt_key
  curl -fsSL $1 -o $tmp_file           || err "cant download $3"
  rm -f $2                             ## clean up target key if exist
  if [[ $(file $tmp_file) =~ \(old\) ]]
  then
    # old format
    cat $tmp_file  | gpg --no-tty --batch --dearmor -o $2 >/dev/null || err "cant import key '"$tmp_file"' to gpg keyring file: $2"
    echo "ver: old"
  else
    # assume new format: OpenPGP Public Key Version 4, Created Wed Nov 15 13:33:00 2023, RSA (Encrypt or Sign, 4096 bits); User ID; Signature; OpenPGP Certificate
    cat $tmp_file | tee $2 > /dev/null
    echo "ver: $(file $tmp_file | grep -oP 'Version\s*\K[^,]+')"
  fi
  rm -Rf $tmp_dir
}

function _apt_repo_add {
  say  "Install repo"
  echo "src: $1"
  echo "dst: $2"
  echo "sig: $3"

  echo "deb [signed-by=$3] $1"    > $2 || err "cant write to $2"
}

function _service_wait {
  say "Enable service $2"
  systemctl daemon-reload
  systemctl -q is-enabled $2 || systemctl enable $2
  systemctl restart $2

  say "Wait up to $1 seconds for service $2 up"
  for ((i=1; i<=$1; i++))
  do
    systemctl is-active --quiet $2 && echo OK && return 2
  done

  echo "Service $2 cannot be started:"
  systemctl status $2
  exit 1
}

# Usage example:
#   printf "x%s_%s\n" \
#    "$(lsb_search "ID")" \
#    "$(lsb_search "VERSION_ID")"
# Output:
#   xFedora_38
#
function _lsb_search {
   while IFS="\n" read -r i
   do
     if [[ $i =~ ^$1=(.+) ]]
     then
       local result="${BASH_REMATCH[1]}"
       if [[ $1 =~ ^ID$ ]] && [[ $(cat /etc/*-release) =~ Ubuntu ]]
       then
         echo "x${result^}"
       else
         echo "${result//\"}"
       fi
       break
     fi
   done <<< "$(cat /etc/*-release 2>/dev/null)"
}

function _packages_txt_search {
   while IFS="\n" read -r i
   do
     if [[ $i =~ ^$1=[[:space:]]*(.+) ]]
     then
       if [[ -n $2 ]]; then
         [[ ${BASH_REMATCH[1]} =~ ($2) ]] && \
         echo "${BASH_REMATCH[1]}"
         return
       else
         echo "${BASH_REMATCH[1]}"
         return
       fi
     fi
   done <<< "$(cat ./packages.txt)"
}

function _var_validate {
  say "Inspect variables"
  local failed=0
  for i in "$@"
  do
    echo $i=${!i}
    [[ ! -n ${!i} ]] && failed=1
  done
  if [[ $failed -eq 1 ]]
  then
    err "Error: undefined variable in _var_validate"
    exit 1
  fi
}

# Must be start from folder with packages.txt
function _packages_download {
    local txt=$1
    local pkg=$2
    local deb_files_version=""
    local is_err=""

    _var_validate PWD pkg txt

    say "Create pkg dir: $pkg"
    mkdir -p $pkg

    say "Cat $txt"
    cat $txt

    say "Looking for available package versions"
    for i in "$(grep -vE '^(#|$)' $txt)"
    do
      [[ $i =~ ^# ]] && continue
      apt-cache madison ${i%%=*} \
        | sed "s|${i##*=}|$(info ${i##*=})|" \
        || true
    done \
    | sort -V -k2

    say "Download packages"
    apt-get install \
      --yes \
      --download-only $(grep -vE '^(#|$)' $txt) \
      --reinstall \
      --no-install-recommends \
      --no-install-suggests \
      --option Dir::Cache::archives=$pkg \
    || err "Error: cannot download packages"

    say "Parse downloaded files in $pkg"
    for i in $pkg/*.deb
    do
      #*/ Vim syntax fix
      i=${i//%3a/:}
      i=${i%_amd64.deb}
      i=${i%_all.deb}
      i=${i#${pkg}/}
      name="${i%%_*}"
      version="${i#*_}"
      printf -v deb_file_versions "$deb_file_versions\n$name=$version"
    done

    say "Check downloaded files (left column) match names and versions with packages.txt (right column)"
    diff="$(diff -y <(printf "$deb_file_versions\n" | grep -vE '^(#|$)') <(sort ./packages.txt | grep -vE '^(#|$)') || true)"
    while IFS='\n' read -r i
    do
      if [[ $i =~ \| ]]
      then
        errnotexit "$i"
        is_err=1
      else
        info "$i"
      fi
    done <<< "$diff"

    if [[ -n $is_err ]]
    then
        err "Error: version mismatch between downloaded files and packages.txt"
        exit 1
    fi

    echo OK
}

function version_compare {
   ver1=$(version_parse $1)
   ver2=$(version_parse $3)
   if [[ $ver1 -ge $ver2 ]]
   then
     echo OK
   else
     err fail
   fi
}

function version_parse {
  echo "$@" | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }'
}

function sysctl-set {
  local dst="$1"
  shift
  echo "$@" > $dst
  chmod 644 $dst
  sysctl -f $dst || err "Failed to set sysctl: $@"
}

function service-off {
  while read -r i
  do
    if systemctl -q is-enabled $i 2>/dev/null \
    || systemctl -q is-active  $i 2>/dev/null
    then
      systemctl disable --now $i 2>&1
      # | grep -vE '(service does not exist)'
      systemctl mask $i
    fi
  done <<< "$@"
}

function yml_module_check {
  local list="$1"
  local name="$2"
  shift 2
  local modules="$@"
  local my_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

  # check no more modules exists with a same digital prefix
  while read -r m; do # check
    if [[ $m =~ ${name%-*}- ]]; then
      >&2 echo "Error: modules '$m' and '$name' shared the same prefix '${name%-*}' which must be mutually exclusive, in $list"
      return 2
    fi
  done <<< "$modules"

  # check modules listed in yml har module folders on filesystem
  if [[ ! -d $my_dir/$name ]]; then
    >&2 echo "Error: directory '$my_dir/$name' not found for module '$name' in $list"
    return 3
  fi
}

function module-set {
  local dst=$1
  shift
  if [[ -f /.dockerenv ]]
  then
    warn "skip swapoff due running from docker"
    return
  fi
  echo "$@" > $dst
  chmod 644 $dst
  modprobe $(cat $dst) || err "Failed to load module from $dst "
}

# read from yml and return value or list of values,
# Next lyml (light yaml) syntaxes is only supported:
# $ yml_read /path/to/file.yml section_name # to get list of values
# section_name:
#   - 010-value_is_list_element1
#   - 020-value_is_list elementx
# yml_read /path/to/file.yml section_name var_name1 # to get one value
# section_name:
#   var_name1: value1
#   var_name2: value2
function yml_read {
  local list_file=$1
  local section=$2:
  local varname=$3:
  local output=""

  while read -r i j; do
    if   [[ $i =~ ^#  ]]; then continue
    elif [[ $i == "-" ]]; then type="listelement" name="$j" value=""
    elif [[   -n $j   ]]; then type="subvariable" name="$i" value="$j"
    elif [[ ! -n $j   ]]; then type="chaptername" name="$i" value=""   current_chapter="$i"
    fi

    if [[ ! $current_chapter == $section ]]; then
      continue
    fi

    if   [[ $type == subvariable ]] && [[ $varname == $name ]]; then
      output="${j#\"*}"
      output="${output%%\"*}\n"
      break
    elif [[ $type == listelement ]]; then
      yml_module_check $list_file $name $output || return 2
      output+="$j\n"
  fi
  done < "$list_file"

  if [[ -n $output ]]; then
    printf "$output"
  else
    >&2 echo "Error: yml_read cannot read value of: '$section $varname' from $list_file"
    >&2 echo "$(cat $list_file)"
    return 1
  fi
}

function _add_alias {
  chk="$1"
  add="$2"

  if ! grep -qa "$1" ~/.bashrc
  then
    echo "$2"   >> ~/.bashrc
  fi
}
