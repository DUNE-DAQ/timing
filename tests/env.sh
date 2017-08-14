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

CACTUS_ROOT=/opt/cactus
PDT_TESTS=$( readlink -f $(dirname $BASH_SOURCE)/ )
PDT_ROOT=$( readlink -f ${PDT_TESTS}/.. )

pathadd PATH "${PDT_ROOT}/core/bin"
pathadd PATH "${PDT_ROOT}/tests/bin"
pathadd PATH "${PDT_ROOT}/tests/scripts"
pathadd LD_LIBRARY_PATH "${PDT_ROOT}/core/lib"

# add python path
pathadd PYTHONPATH "${PDT_ROOT}/python/pkg"
pathadd PYTHONPATH "${PDT_ROOT}/tests/python"


export PATH LD_LIBRARY_PATH PYTHONPATH
export PDT_ROOT PDT_TESTS


eval "$(_PDTBUTLER_COMPLETE=source pdtbutler)"


