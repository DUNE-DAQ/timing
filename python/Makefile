PDT_ROOT= $(shell pwd)/..

BUILD_HOME = $(PDT_ROOT)
include ${PDT_ROOT}/config/mfCommonDefs.mk

Project = pdt
Package = python

PackagePath = $(PDT_ROOT)/python
PackageName = protodune-pdt-python

PythonModules = ["pdt"]
$(info PythonModules=${PythonModules})

Packager = Alessandro Thea

PACKAGE_VER_MAJOR = 4
PACKAGE_VER_MINOR = 0
PACKAGE_VER_PATCH = 0
PACKAGE_RELEASE = 0

PackageRequires = cactuscore-uhal-uhal >= 2.5.0, cactuscore-uhal-uhal < 2.6.0
PackageBuildRequires=${PackageRequires}

# Rationale for -Wno-deprecated-declarations: If we're using the UPS
# version of uhal, along with its newer compiler and newer boost, we
# get deprecation warnings for auto_ptr, so quieten those down
#
# https://svn.boost.org/trac10/ticket/11622

CxxFlags += \
		-ftemplate-depth-128 \
		-O0 -rdynamic -finline-functions \
		-Wno-inline -Wno-deprecated-declarations \
		-DNDEBUG

Library = protodune_pdt_python

# The leading undescore is meant to be there
LibraryTarget = pkg/pdt/_pdt.so


Includes = \
    include \
    $(PYTHON_INCLUDE_PREFIX) \
    $(PDT_ROOT)/core/include \
    $(CACTUS_INC_DIRS)

Libraries = \
	boost_python \
	protodune_pdt_core 

LibraryPaths = \
	$(PDT_ROOT)/core/lib \
        $(CACTUS_LIB_DIRS)


include ${PDT_ROOT}/config/mfRules.mk
include ${PDT_ROOT}/config/mfPythonRPMRules.mk
