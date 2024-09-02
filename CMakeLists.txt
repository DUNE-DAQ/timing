cmake_minimum_required(VERSION 3.12)
project(timing VERSION 7.6.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(ers REQUIRED)     
find_package(logging REQUIRED)
find_package(pybind11 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(opmonlib REQUIRED)
find_package(uhal REQUIRED)


daq_codegen( timingfirmware.jsonnet timingfirmwareinfo.jsonnet timinghardwareinfo.jsonnet timingendpointinfo.jsonnet TEMPLATES Structs.hpp.j2 Nljs.hpp.j2 )
##############################################################################
daq_add_library(*.cpp LINK_LIBRARIES ers::ers logging::logging nlohmann_json::nlohmann_json uhal::uhal opmonlib::opmonlib)

##############################################################################
daq_add_python_bindings(*.cpp LINK_LIBRARIES ${PROJECT_NAME})


##############################################################################
daq_install()
