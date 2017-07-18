# Sanitize BUILD_HOME
BUILD_HOME := ${shell cd ${BUILD_HOME}; pwd}

$(info Using BUILD_HOME=${BUILD_HOME})

export BUILD_HOME

# Cactus config. This section shall be sources from /opt/pdf/config
PDT_SYSROOT ?= /opt/pdt

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

$(info OS detected: $(PDT_OS))

## Environment
# Make sure $PDT_SYSROOT/lib is present in LD_LIBRARY_PATH

ifeq ($(findstring $(PDT_SYSROOT)/lib,$(LD_LIBRARY_PATH)),)
    $(info PDT_SYSROOT/lib added to LD_LIBRARY_PATH)
    LD_LIBRARY_PATH:=$(PDT_SYSROOT)/lib:$(LD_LIBRARY_PATH)
else
    $(info PDT_SYSROOT already in LD_LIBRARY_PATH)
endif

export LD_LIBRARY_PATH

#$(info Linker path: $(LD_LIBRARY_PATH))
VERBOSE ?= 0

# Compilers
CPP:=g++
LD:=g++

## Tools
MakeDir=mkdir -p

# Formatting
At_0 := @
At_1 := 
At = ${At_${VERBOSE}}

ColorGreen := \033[0;32m
ColorNone := \033[0m

CPP_Actual := ${CPP}
CPP_0 = @echo -e "-> Compiling ${ColorGreen}$(<F)${ColorNone}..."; ${CPP_Actual}
CPP_1 = ${CPP_Actual}
CPP = ${CPP_${VERBOSE}}

LD_Actual := ${LD}
LD_0 = @echo -e "-> Linking ${ColorGreen}$(@F)${ColorNone}..."; ${LD_Actual}
LD_1 = ${LD_Actual}
LD = ${LD_${VERBOSE}}

MakeDir_Actual := ${MakeDir}
MakeDir = ${At} ${MakeDir_Actual}

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

