#!/bin/bash

usage()
{
    echo "Usage:" >&2
    echo "$(basename $0) -v version -b builddir -i installdir -q qualifiers" >&2
    echo
    echo " -v version:     software version string for ups"                   >&2
    echo " -u uhalversion: required version of uhal"                          >&2
    echo " -b builddir:    the root directory of where the sw was built"      >&2
    echo " -i installdir:  the ups products directory to install into"        >&2
    echo " -q qualifiers:  the ups qualifiers for this build"                 >&2
}

while getopts "b:i:u:v:q:h" opt; do
    case $opt in
	b)
	    # Where the timing board software was built
	    BUILDDIR="${OPTARG}"
	    ;;
	i)
	    # The products directory we want to install the UPS version in
	    MYPRODDIR="${OPTARG}"
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

if [ -z "$UHALVERSION" ]; then
    echo "uhal version not set. Use -u" >&2
    usage
    exit 1
fi

# Setup UPS, since we'll be needing it later
. /nfs/sw/artdaq/products/setups
# We use get-directory-name from cetpkgsupport below
setup -c cetpkgsupport

# BUILDDIR=/nfs/home/phrodrig/protodune/timing/upsify/timing-board-software
# MYPRODDIR=/nfs/home/phrodrig/protodune/timing/upsify/myproducts

# The name of the UPS product we're making 
PRODNAME=protodune_pdt_core
# VERSION=v0_0_1
# The necessary gubbins to find the directory name for the build in
# UPS, so we get something like "slf7.x86_64.e14.prof.s50"
SUBDIR=`get-directory-name subdir` || exit 1
# Sometimes we need the qualifiers colon-separated, sometimes
# dot-separated, so form both
QUALSDIR=$(echo ${QUALS} | tr ':' '.')

# Is my installation dir already in the places where UPS will look? If not, add it
if ! echo $PRODUCTS | grep -q ${MYPRODDIR}; then 
    export PRODUCTS=$PRODUCTS:${MYPRODDIR}
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

TABLE="${VERSIONDIR}/ups/${PRODNAME}.table"

# Not sure what the best thing to do if the product is already built
# in this configuration. This will hopefully at least cause 'make ups'
# to not fail
if ups exist ${PRODNAME} ${VERSION} -q ${QUALS}; then
    echo
    echo Product already exists with this version. I will undeclare it and re-declare
    echo You may want to manually remove ${VERSIONDIR}/${SUBDIR}.${QUALSDIR} or even
    echo ${VERSIONDIR} and rerun 'make ups'
    echo 
    ups undeclare -z ${MYPRODDIR} -r ${VERSIONDIR} -5 -m ${TABLE} -q ${QUALS} ${PRODNAME} ${VERSION}
fi

ORIG_TABLE="${BUILDDIR}/config/${PRODNAME}.table"
# Replace the version string in the table file with the version we were told on the command line
sed -e "s,__VERSION__,${VERSION},g" -e "s,__REQUIRED_UHAL_VERSION__,${UHALVERSION},g" ${ORIG_TABLE} > "${TABLE}"

echo Declaring with:
echo ups declare -z ${MYPRODDIR} -r ${VERSIONDIR} -5 -m ${TABLE} -q ${QUALS} ${PRODNAME} ${VERSION}
     ups declare -z ${MYPRODDIR} -r ${VERSIONDIR} -5 -m ${TABLE} -q ${QUALS} ${PRODNAME} ${VERSION}

if [ "$?" != "0" ]; then
    echo ups declare failed. Bailing
    exit 1
fi

echo Now ups list says the available builds are:
ups list -aK+ protodune_pdt_core

setup ${PRODNAME} ${VERSION} -q ${QUALS}

if [ "$?" != "0" ]; then
    echo setup failed. Bailing
    exit 1
else
    echo Setting up the new product succeeded
fi

if [ ! -d "${PROTODUNE_PDT_CORE_INC}" ]; then
    echo \$PROTODUNE_PDT_CORE_INC is not defined or does not exist. I don\'t know what to do
    exit 1
fi

if [ ! -d "${PROTODUNE_PDT_CORE_LIB}" ]; then
    echo \$PROTODUNE_PDT_CORE_LIB is not defined or does not exist. I don\'t know what to do
    exit 1
fi

# Now that everything's set up, we can actually copy the files we want
# into the install directory
cp -r ${BUILDDIR}/core/include/* ${PROTODUNE_PDT_CORE_INC}
cp -r ${BUILDDIR}/core/lib/* ${PROTODUNE_PDT_CORE_LIB}
