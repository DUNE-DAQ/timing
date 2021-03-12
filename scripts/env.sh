function pathadd() {
  # TODO add check for empty path
  # and what happens if $1 == $2
  # Copy into temp variables
  PATH_NAME=$1
  PATH_VAL=${!1}
  if [[ ":$PATH_VAL:" != *":$2:"* ]]; then
    PATH_VAL="$2${PATH_VAL:+":$PATH_VAL"}"
    echo "- $1 += $2"

    # use eval to reset the target
    eval "$PATH_NAME=$PATH_VAL"
  fi

}


declare -a missing_pypkg

function chkpypkg() {
  if python -c "import pkgutil; raise SystemExit(1 if pkgutil.find_loader('${1}') is None else 0)" &> /dev/null; then
    echo "${1} is installed"
else
    echo "Error: package '${1}' is not installed"
    missing_pypkg+=(${1})
fi
}

chkpypkg uhal
chkpypkg click
chkpypkg click_didyoumean

if (( ${#missing_pypkg[@]} > 0 )); then
  echo "Aborting."
  unset missing_pypkg
  return 1
fi
unset missing_pypkg

TIMING_SHARE=$( readlink -f $(dirname $BASH_SOURCE)/../ )
export TIMING_SHARE

eval "$(_PDTBUTLER_COMPLETE=source pdtbutler)"


