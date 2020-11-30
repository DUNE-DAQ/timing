PDT_ROOT= $(shell pwd)/..

BUILD_HOME = $(PDT_ROOT)
include ${PDT_ROOT}/config/mfCommonDefs.mk

Project = pdt
Package = core

PackagePath = $(PDT_ROOT)/core
PackageName = protodune-pdt-core

Packager = Alessandro Thea

PACKAGE_VER_MAJOR = 6
PACKAGE_VER_MINOR = 0
PACKAGE_VER_PATCH = 0b0
PACKAGE_RELEASE = 0

# Requirements for the rpm build
PackageRequires = cactuscore-uhal-uhal >= 2.5.0, cactuscore-uhal-uhal < 2.6.0
PackageBuildRequires = ${PackageRequires}

Library = protodune_pdt_core

Includes = \
    include \
    $(CACTUS_INC_DIRS)

Libraries = \
    boost_date_time \
    boost_system \
    cactus_uhal_log \
    cactus_uhal_uhal

#LibraryPaths = \
#  $(CACTUS_ROOT)/lib 

ExecutableLibraries = ${Libraries} \
	${Library} \
    boost_filesystem \
    boost_thread

LibraryPaths = \
	${CACTUS_LIB_DIRS} \
	${PDT_ROOT}/core/lib

include ${PDT_ROOT}/config/mfRules.mk
include ${PDT_ROOT}/config/mfRPMRules.mk

