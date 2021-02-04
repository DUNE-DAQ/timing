cmake_minimum_required(VERSION 3.12)
project(timing-board-software VERSION 7.0.0)

##############################################################################

set(PYTHON_LIB_NAME _pdt)
file(GLOB PYTHON_LIB_SOURCES "src/common/*.cpp")
add_library(${PYTHON_LIB_NAME} SHARED ${PYTHON_LIB_SOURCES} )

set(LIST_OF_BOOST_LIBS $ENV{BOOST_LIB}/libboost_python38.so) # list of UHAL libs.

target_link_libraries(${PYTHON_LIB_NAME} PUBLIC ${LIBOPTS_LINK_LIBRARIES} ${PROJECT_NAME} ${LIST_OF_BOOST_LIBS} )

#boost_python boost_system boost_system-mt

target_include_directories(${PYTHON_LIB_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> $ENV{PYTHON_INCLUDE})
_daq_set_target_output_dirs( ${PYTHON_LIB_NAME} src )

_daq_define_exportname()
install(TARGETS ${PYTHON_LIB_NAME} EXPORT ${DAQ_PROJECT_EXPORTNAME} )
set(DAQ_PROJECT_INSTALLS_TARGETS true PARENT_SCOPE)
