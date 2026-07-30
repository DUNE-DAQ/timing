cmake_minimum_required(VERSION 3.12)
project(timing VERSION 7.14.0)

find_package(daq-cmake REQUIRED)

daq_setup_environment()

find_package(Boost COMPONENTS filesystem format thread REQUIRED)
find_package(ers REQUIRED)     
find_package(logging REQUIRED)
find_package(pybind11 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(opmonlib REQUIRED)
find_package(uhal REQUIRED)


##############################################################################
daq_add_library(*.cpp LINK_LIBRARIES ers::ers logging::logging nlohmann_json::nlohmann_json uhal::uhal opmonlib::opmonlib Boost::filesystem Boost::format Boost::thread)

##############################################################################
daq_add_python_bindings(*.cpp)


##############################################################################
daq_install()
