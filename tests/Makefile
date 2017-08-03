PDT_ROOT= $(shell pwd)/..

BUILD_HOME = $(PDT_ROOT)
include ${PDT_ROOT}/config/mfCommonDefs.mk

Project = pdt
Package = tests

PackagePath = $(PDT_ROOT)/tests
PackageName = protodune-pdt-tests

Packager = Alessandro Thea

PACKAGE_VER_MAJOR = 0
PACKAGE_VER_MINOR = 1
PACKAGE_VER_PATCH = 0
PACKAGE_RELEASE = 0

PackageRequires = cactuscore-uhal-uhal >= 2.5.0, cactuscore-uhal-uhal < 2.6.0
PackageBuildRequires = ${PackageRequires}

Library = protodune_pdt_core

Includes = \
    include \
	${PDT_ROOT}/core/include \
	$(CACTUS_ROOT)/include 

Libraries = \
    boost_date_time \
    boost_system \
    cactus_uhal_log \
    cactus_uhal_uhal

LibraryPaths = \
  $(CACTUS_ROOT)/lib 

ExecutableLibraries = ${Libraries} \
	${Library} \
    boost_filesystem \
    boost_thread

LibraryPaths = \
	${CACTUS_ROOT}/lib \
	${PDT_ROOT}/core/lib

include ${PDT_ROOT}/config/mfRules.mk
include ${PDT_ROOT}/config/mfRPMRules.mk

