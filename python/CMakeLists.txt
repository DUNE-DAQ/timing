cmake_minimum_required(VERSION 3.12)
project(timing VERSION 7.0.0)

##############################################################################
set(PYTHON_LIB_NAME core)

file(GLOB PYTHON_LIB_SOURCES "timing/core/src/*.cpp")
pybind11_add_module(${PYTHON_LIB_NAME} ${PYTHON_LIB_SOURCES})

target_link_libraries(${PYTHON_LIB_NAME} PUBLIC ${PROJECT_NAME}) 
target_include_directories(${PYTHON_LIB_NAME} PUBLIC $ENV{UHAL_INC})
set_target_properties(${PYTHON_LIB_NAME} PROPERTIES SUFFIX ".so")

_daq_set_target_output_dirs(${PYTHON_LIB_NAME} timing/core )


_daq_define_exportname()
install(TARGETS ${PYTHON_LIB_NAME} EXPORT ${DAQ_PROJECT_EXPORTNAME} DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/timing/core )
set(DAQ_PROJECT_INSTALLS_TARGETS true PARENT_SCOPE)
