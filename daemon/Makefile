PDT_ROOT= $(shell pwd)/..

BUILD_HOME = $(PDT_ROOT)
include ${PDT_ROOT}/config/mfCommonDefs.mk

Project = pdt
Package = daemon

PackagePath = $(PDT_ROOT)/daemon
PackageName = protodune-pdt-daemon

Packager = Alessandro Thea

PACKAGE_VER_MAJOR = 0
PACKAGE_VER_MAJOR = 0
PACKAGE_VER_PATCH = 0
PACKAGE_RELEASE = 0

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
    cactus_uhal_uhal \
    protodune_pdt_core

ExecutableLibraries = ${Libraries} \
	${Library} \
    boost_filesystem \
    boost_thread

LibraryPaths = \
	${CACTUS_LIB_DIRS} \
	${PDT_ROOT}/core/lib \
	${PDT_ROOT}/daemon/lib

include ${PDT_ROOT}/config/mfRules.mk
include ${PDT_ROOT}/config/mfRPMRules.mk

