cmake_minimum_required(VERSION 3.12)
project(timing-board-software VERSION 7.0.0)

##############################################################################

set(PYTHON_LIB_NAME _pdt)
add_library(${PYTHON_LIB_NAME} SHARED src/common/core_wrapper.cpp )
target_link_libraries(${PYTHON_LIB_NAME} PUBLIC ${LIBOPTS_LINK_LIBRARIES} ${PROJECT_NAME})
target_include_directories(${PYTHON_LIB_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> $ENV{PYTHON_INCLUDE})
_daq_set_target_output_dirs( ${PYTHON_LIB_NAME} src )

_daq_define_exportname()
install(TARGETS ${PYTHON_LIB_NAME} EXPORT ${DAQ_PROJECT_EXPORTNAME} )
set(DAQ_PROJECT_INSTALLS_TARGETS true PARENT_SCOPE)
