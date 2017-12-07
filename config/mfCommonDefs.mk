# Sanitize BUILD_HOME
BUILD_HOME := ${shell cd ${BUILD_HOME}; pwd}

$(info Using BUILD_HOME=${BUILD_HOME})

export BUILD_HOME

# PDT_SYSROOT
PDT_SYSROOT = /opt/pdt
DEBUG ?= 1


# OS identification
PDT_PLATFORM=$(shell /usr/bin/python -c "import platform; print platform.platform()")
PDT_OS=unknown.os
UNAME=$(strip $(shell uname -s))
ifeq ($(UNAME),Linux)
    ifneq ($(findstring centos-6,$(PDT_PLATFORM)),)
        PDT_OS=centos6
    else ifneq ($(findstring centos-7,$(PDT_PLATFORM)),)
        PDT_OS=centos7
    endif
else ifeq ($(UNAME),Darwin)
    PDT_OS=osx
endif

$(info Detected OS $(PDT_OS))

#$(info Linker path: $(LD_LIBRARY_PATH))
VERBOSE ?= 0

# Compilers
CPP:=g++
LD:=g++

## Tools
MakeDir=mkdir -p
Cp=cp
RpmBuild=rpmbuild
Sed=sed


# Formatting
At_0 := @
At_1 := 
At = ${At_${VERBOSE}}

ColorGreen := \033[0;32m
ColorBlue := \033[0;34m
ColorNone := \033[0m

CPP_Actual := ${CPP}
CPP_0 = @echo -e "(${ColorBlue}${Project}/${Package}${ColorNone}) -> Compiling ${ColorGreen}$(<F)${ColorNone}..."; ${CPP_Actual}
CPP_1 = ${CPP_Actual}
CPP = ${CPP_${VERBOSE}}

LD_Actual := ${LD}
LD_0 = @echo -e "(${ColorBlue}${Project}/${Package}${ColorNone}) -> Linking ${ColorGreen}$(@F)${ColorNone}..."; ${LD_Actual}
LD_1 = ${LD_Actual}
LD = ${LD_${VERBOSE}}

MakeDir_Actual := ${MakeDir}
MakeDir = ${At} ${MakeDir_Actual}

Cp_Actual := ${Cp}
Cp = ${At} ${Cp_Actual}

RpmBuild_Actual := ${RpmBuild}
RpmBuild_0 = @echo -e "-> Building ${ColorGreen}RPM${ColorNone}..."; ${RpmBuild_Actual}
RpmBuild_1 = ${RpmBuild_Actual}
RpmBuild = ${At} ${RpmBuild_${VERBOSE}}

Sed_Actual := ${Sed}
Sed = ${At} ${Sed_Actual}


## Python
PYTHON_VERSION ?= $(shell python -c "import distutils.sysconfig;print distutils.sysconfig.get_python_version()")
PYTHON_INCLUDE_PREFIX ?= $(shell python -c "import distutils.sysconfig;print distutils.sysconfig.get_python_inc()")

## Compiler flags
ifndef DEBUG
    CxxFlags = -g -Wall -Werror=return-type -O3 -MMD -MP -fPIC -std=c++11
    LinkFlags = -g -shared -fPIC -Wall -O3 
    ExecutableLinkFlags = -g -Wall -O3
else
    CxxFlags = -g -ggdb -Wall -Werror=return-type -MMD -MP -fPIC -std=c++11
    LinkFlags = -g -ggdb -shared -fPIC -Wall
    ExecutableLinkFlags = -g -ggdb -Wall -std=c++11
endif

# We want to be able to build against both the system cactus library
# in /opt/cactus and the one in ups. This would be easy, except the
# two have different directory layouts. So we set the paths depending
# on whether USE_UPS_CACTUS is set from the command line
ifdef USE_UPS_CACTUS
    # Check that uhal has been setup by ups
    ifndef UHAL_INC
        $(error UHAL_INC is not set. Make sure uhal has been setup via ups)
    endif
    ifndef UHAL_LIB
        $(error UHAL_LIB is not set. Make sure uhal has been setup via ups)
    endif
    CACTUS_INC_DIRS := $(UHAL_INC) $(UHAL_INC)/uhal $(BOOST_INC)
    $(info Using CACTUS_INC_DIRS=${CACTUS_INC_DIRS})
    CACTUS_LIB_DIRS := $(UHAL_LIB) $(BOOST_LIB)
    $(info Using CACTUS_LIB_DIRS=${CACTUS_LIB_DIRS})
else 
    CACTUS_ROOT ?= /opt/cactus
    CACTUS_INC_DIRS := $(CACTUS_ROOT)/include
    CACTUS_LIB_DIRS := $(CACTUS_ROOT)/lib

    # Environment
    # Ensure that $CACTUS_ROOT/lib is present in LD_LIBRARY_PATH
    ifeq ($(findstring $(CACTUS_ROOT)/lib,$(LD_LIBRARY_PATH)),)
        $(info CACTUS_ROOT/lib added to LD_LIBRARY_PATH)
        LD_LIBRARY_PATH:=$(CACTUS_ROOT)/lib:$(LD_LIBRARY_PATH)
    else
        $(info CACTUS_ROOT already in LD_LIBRARY_PATH)
    endif

    export LD_LIBRARY_PATH

endif
