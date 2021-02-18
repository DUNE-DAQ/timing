#!/bin/bash

usage()
{
    echo "Usage:" >&2
    echo "$(basename $0) -v version -b builddir -i installdir -q qualifiers" >&2
    echo
    echo " -p product:     name of ups product"                               >&2
    echo " -v version:     software version string for ups"                   >&2
    echo " -u uhalversion: required version of uhal"                          >&2
    echo " -b builddir:    the root directory of where the sw was built"      >&2
    echo " -i installdir:  the ups products directory to install into"        >&2
    echo " -q qualifiers:  the ups qualifiers for this build"                 >&2
}

while getopts "p:b:i:u:v:q:h" opt; do
    case $opt in
	b)
	    # Where the timing board software was built
	    BUILDDIR="${OPTARG}"
	    ;;
	i)
	    # The products directory we want to install the UPS version in
	    MYPRODDIR="${OPTARG}"
	    ;;
	p)
	    # The product name
	    PRODNAME="${OPTARG}"
	    ;;
	v)
	    # The version string for UPS
	    VERSION="${OPTARG}"
	    ;;
	u)
	    # The version of uhal we depend on
	    UHALVERSION="${OPTARG}"
	    ;;
	q)
	    # The qualifiers for UPS, colon-separated
	    QUALS="${OPTARG}"
	    ;;
	h)
	    usage
	    exit 0
	    ;;
	\?)
	    echo "Invalid option: -$OPTARG" >&2
	    usage
	    exit 1
	    ;;
	:)
	    echo "Option -$OPTARG requires an argument." >&2
	    usage
	    exit 1
	    ;;
    esac
done

# Check everything we needed got set to something sane
if [ -z "$BUILDDIR" ]; then
    echo "Build directory not set. Use -b" >&2
    usage
    exit 1
fi

if [ ! -d "$BUILDDIR" ]; then
    echo "Build directory \"${BUILDDIR}\" nonexistent or not a directory" >&2
    usage
    exit 1
fi

if [ -z "$MYPRODDIR" ]; then
    echo "Install directory not set. Use -i" >&2
    usage
    exit 1
fi

if [ ! -d "$MYPRODDIR" ]; then
    echo "Install directory \"${MYPRODDIR}\" nonexistent or not a directory" >&2
    usage
    exit 1
fi

if [ -z "$QUALS" ]; then
    echo "UPS qualifiers not set. Use -q" >&2
    usage
    exit 1
fi

if [ -z "$VERSION" ]; then
    echo "Version not set. Use -v" >&2
    usage
    exit 1
fi

if [ -z "$PRODNAME" ]; then
    echo "Product name not set. Use -p" >&2
    usage
    exit 1
fi

if [ -z "$UHALVERSION" ]; then
    echo "uhal version not set. Use -u" >&2
    usage
    exit 1
fi

# Make sure UPS is set up, since we'll be using it later
if [ -z "$UPS_DIR" ]; then
    echo "It appears that ups is not set up (\$UPS_DIR is not set). Bailing"
    exit 1
fi

# Unfortunately, ups's 'setup' is a function, and it's not exported to
# subshells, so we can't use it in this script (we *could* manually
# source 'setups', but then we have to hardcode a path here, which is
# ugly). So instead we'll just define setup ourselves, the same way
# that ups does
setup ()
{
    . `ups setup "$@"`
}

# We use get-directory-name from cetpkgsupport below
if ! ups active | grep -q cetpkgsupport; then
    setup -c cetpkgsupport
fi
# We use build_table from cetbuildtools to make the table file from the product_deps file
if ! ups active | grep -q cetbuildtools; then
    setup cetbuildtools v7_02_01
fi

# Sort the qualifiers in the same way as build_table does:
# 'prof'/'debug' at the end, and the rest in alphabetical order
function sort_quals
{
    ret=""
    optimization=""
    for i in $(echo $1 | tr ':' '\n' | sort); do
        if [[ $i = "prof" || $i = "debug" ]]; then
            optimization="$i"
        else
            ret="$ret $i"
        fi
    done
    ret="$ret $optimization"
    # Use printf not echo deliberately here: $ret gets split into
    # words, and printf prints each as "%s\n", so now there are line
    # breaks between the quals
    printf "%s\n" $ret | paste -sd:
}


# VERSION=v0_0_1
# The necessary gubbins to find the directory name for the build in
# UPS, so we get something like "slf7.x86_64.e14.prof.s50"
SUBDIR=`get-directory-name subdir` || exit 1
# Sometimes we need the qualifiers colon-separated, sometimes
# dot-separated, so form both
QUALS=$(sort_quals ${QUALS})
QUALSDIR=$(echo ${QUALS} | tr ':' '.')

# Is my installation dir already in the places where UPS will look? If not, add it
if ! echo $PRODUCTS | grep -q ${MYPRODDIR}; then 
    export PRODUCTS=$PRODUCTS:${MYPRODDIR}
fi

# For whatever reason, UPS needs this .upsfiles/dbconfig file to be
# present at the top-level of a $PRODUCTS directory in order to allow
# relocatable products to be installed there. So create it
if [ ! -e "${MYPRODDIR}/.upsfiles/dbconfig" ]; then
    mkdir -p "${MYPRODDIR}/.upsfiles" || exit 1
    cat > "${MYPRODDIR}/.upsfiles/dbconfig"  <<EOF 
FILE     = DBCONFIG
AUTHORIZED_NODES = *
VERSION_SUBDIR = 1
PROD_DIR_PREFIX = \${UPS_THIS_DB}
UPD_USERCODE_DIR = \${UPS_THIS_DB}/.updfiles

EOF
fi

# Make the directories that UPS wants, and do the minimum necessary to
# be able to setup the product (ie, copy across the table file). Then
# actually setup the product so that we can copy the files into the
# directories where UPS thinks they should be. This is really a bit of
# extra paranoia, since we basically form those directories ourselves,
# but it allows us to check that everything is working
VERSIONDIR="${MYPRODDIR}/${PRODNAME}/${VERSION}"
mkdir -p "${VERSIONDIR}/ups"                       || exit 1
mkdir -p "${VERSIONDIR}/include"                   || exit 1
mkdir -p "${VERSIONDIR}/${SUBDIR}.${QUALSDIR}"     || exit 1
mkdir -p "${VERSIONDIR}/${SUBDIR}.${QUALSDIR}/lib" || exit 1

ORIG_PRODUCT_DEPS="${BUILDDIR}/config/product_deps.template"

# Replace the version string, etc, in the table file with the version we were told on the command line
sed -e "s,__PRODUCT__,${PRODNAME},g" \
    -e "s,__VERSION__,${VERSION},g" \
    -e "s,__REQUIRED_UHAL_VERSION__,${UHALVERSION},g" \
    ${ORIG_PRODUCT_DEPS} > "${VERSIONDIR}/ups/product_deps" || exit 1

# Make the table file from the product_deps file
build_table ${VERSIONDIR}/ups ${VERSIONDIR}/ups || exit 1

TABLE="${PRODNAME}.table"

# Not sure what the best thing to do if the product is already built
# in this configuration. This will hopefully at least cause 'make ups'
# to not fail
if ups exist -z ${MYPRODDIR} ${PRODNAME} ${VERSION} -q ${QUALS}; then
    echo
    echo Product already exists with this version. I will undeclare it and re-declare
    echo You may want to manually remove ${VERSIONDIR}'/*' or even
    echo ${VERSIONDIR} and rerun 'make ups'
    echo 
    ups undeclare -z ${MYPRODDIR} -r ${VERSIONDIR} -5 -m ${TABLE} -q ${QUALS} ${PRODNAME} ${VERSION}
fi


echo Declaring with:
echo ups declare -z ${MYPRODDIR} -r ${VERSIONDIR} -5 -m ${TABLE} -q ${QUALS} ${PRODNAME} ${VERSION}
     ups declare -z ${MYPRODDIR} -r ${VERSIONDIR} -5 -m ${TABLE} -q ${QUALS} ${PRODNAME} ${VERSION}

if [ "$?" != "0" ]; then
    echo ups declare failed. Bailing
    exit 1
fi

echo Now ups list says the available builds are:
ups list -z ${MYPRODDIR} -aK+ ${PRODNAME}

setup ${PRODNAME} ${VERSION} -q ${QUALS}

if [ "$?" != "0" ]; then
    echo setup failed. Bailing
    exit 1
else
    echo Setting up the new product succeeded
fi

PRODNAME_UC=$(echo $PRODNAME | tr '[:lower:]' '[:upper:]')
PRODNAME_UC_INC="${PRODNAME_UC}_INC"
PRODNAME_UC_LIB="${PRODNAME_UC}_LIB"

if [ ! -d "${!PRODNAME_UC_INC}" ]; then
    echo $PRODNAME_UC_INC is not defined or does not exist. I don\'t know what to do
    exit 1
fi

if [ ! -d "${!PRODNAME_UC_LIB}" ]; then
    echo $PRODNAME_UC_LIB is not defined or does not exist. I don\'t know what to do
    exit 1
fi

# Now that everything's set up, we can actually copy the files we want
# into the install directory
cp -r ${BUILDDIR}/core/include/* ${!PRODNAME_UC_INC}
cp -r ${BUILDDIR}/core/lib/* ${!PRODNAME_UC_LIB}
