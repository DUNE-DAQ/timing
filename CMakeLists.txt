cmake_minimum_required(VERSION 3.12)
project(timing-board-software VERSION 7.0.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

# ers: for exceptions and normal output
# TRACE: for low-level debugging statements

find_package(ers REQUIRED)     
find_package(TRACE REQUIRED)
find_package(Boost 1.70.0 REQUIRED)

##############################################################################
set(LIST_OF_UHAL_LIBS $ENV{UHAL_LIB}/libcactus_uhal_log.so $ENV{UHAL_LIB}/libcactus_uhal_uhal.so) # list of UHAL libs.

##############################################################################
daq_add_library(*.cpp LINK_LIBRARIES ers::ers ${LIST_OF_UHAL_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC $ENV{UHAL_INC})

##############################################################################
add_subdirectory(python)

##############################################################################
daq_install()
