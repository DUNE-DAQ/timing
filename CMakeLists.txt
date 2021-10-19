cmake_minimum_required(VERSION 3.12)
project(timing VERSION 5.6.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(ers REQUIRED)     
find_package(logging REQUIRED)
find_package(Boost 1.73.0 COMPONENTS regex REQUIRED)
find_package(pybind11 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(opmonlib REQUIRED)
find_package(uhal REQUIRED)

daq_codegen( *info.jsonnet DEP_PKGS opmonlib TEMPLATES opmonlib/InfoStructs.hpp.j2 opmonlib/InfoNljs.hpp.j2 )

##############################################################################
daq_add_library(*.cpp LINK_LIBRARIES ers::ers logging::logging nlohmann_json::nlohmann_json uhal::uhal)

##############################################################################
daq_add_python_bindings(*.cpp LINK_LIBRARIES ${PROJECT_NAME})


##############################################################################
daq_install()
