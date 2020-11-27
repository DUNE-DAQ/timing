// Boost Headers
#include <boost/python/def.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/class.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/operators.hpp>

#include "pdt/IONode.hpp"
#include "pdt/FMCIONode.hpp"
#include "pdt/PC059IONode.hpp"
#include "pdt/TLUIONode.hpp"
#include "pdt/SIMIONode.hpp"

// Namespace resolution
using namespace boost::python;

// FMC IO 
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_FMCIONode_reset_overloads, reset, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_FMCIONode_getStatus_overloads, getStatus, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_FMCIONode_getClockFrequenciesTable_overloads, getClockFrequenciesTable, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_FMCIONode_getPLLStatus_overloads, getPLLStatus, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_FMCIONode_getHardwareInfo_overloads, getHardwareInfo, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_FMCIONode_getSFPStatus_overloads, getSFPStatus, 1, 2);

// PC059 IO
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PC059IONode_reset_overloads, reset, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PC059IONode_reset_with_mode_overloads, reset, 1, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PC059IONode_getStatus_overloads, getStatus, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PC059IONode_getClockFrequenciesTable_overloads, getClockFrequenciesTable, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PC059IONode_getPLLStatus_overloads, getPLLStatus, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PC059IONode_getHardwareInfo_overloads, getHardwareInfo, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_PC059IONode_getSFPStatus_overloads, getSFPStatus, 1, 2);

// TLU IO
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_TLUIONode_reset_overloads, reset, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_TLUIONode_configureDAC_overloads, configureDAC, 2, 3);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_TLUIONode_getStatus_overloads, getStatus, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_TLUIONode_getClockFrequenciesTable_overloads, getClockFrequenciesTable, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_TLUIONode_getPLLStatus_overloads, getPLLStatus, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_TLUIONode_getHardwareInfo_overloads, getHardwareInfo, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_TLUIONode_getSFPStatus_overloads, getSFPStatus, 1, 2);

// SIM IO
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_SIMIONode_reset_overloads, reset, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_SIMIONode_getStatus_overloads, getStatus, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_SIMIONode_getClockFrequenciesTable_overloads, getClockFrequenciesTable, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_SIMIONode_getPLLStatus_overloads, getPLLStatus, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_SIMIONode_getHardwareInfo_overloads, getHardwareInfo, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_SIMIONode_getSFPStatus_overloads, getSFPStatus, 1, 2);

namespace pdt {
namespace python {

void
register_io() {

      class_<pdt::IONode , boost::noncopyable>("IONode", no_init);
      
      class_<pdt::FMCIONode, bases<uhal::Node, pdt::IONode> > ("FMCIONode", init<const uhal::Node&>())
      .def("reset", &pdt::FMCIONode::reset, pdt_FMCIONode_reset_overloads())
      .def("softReset", &pdt::FMCIONode::softReset)
      .def("getClockFrequenciesTable", &pdt::FMCIONode::getClockFrequenciesTable, pdt_FMCIONode_getClockFrequenciesTable_overloads())
      .def("getStatus", &pdt::FMCIONode::getStatus, pdt_FMCIONode_getStatus_overloads())
      .def("getPLLStatus", &pdt::FMCIONode::getPLLStatus, pdt_FMCIONode_getPLLStatus_overloads())
      .def("getHardwareInfo", &pdt::FMCIONode::getHardwareInfo, pdt_FMCIONode_getHardwareInfo_overloads())
      .def("getSFPStatus", &pdt::FMCIONode::getSFPStatus, pdt_FMCIONode_getSFPStatus_overloads())
      .def("switchSFPSoftTxControlBit", &pdt::FMCIONode::switchSFPSoftTxControlBit)
      ;

      class_<pdt::PC059IONode, bases<uhal::Node, pdt::IONode> > ("PC059IONode", init<const uhal::Node&>())
      .def<void (pdt::PC059IONode::*)(const std::string&) const>("reset", &pdt::PC059IONode::reset, pdt_PC059IONode_reset_overloads())
      .def<void (pdt::PC059IONode::*)(int32_t,const std::string&) const>("reset", &pdt::PC059IONode::reset, pdt_PC059IONode_reset_with_mode_overloads())
      .def("softReset", &pdt::PC059IONode::softReset)
      .def("getClockFrequenciesTable", &pdt::PC059IONode::getClockFrequenciesTable, pdt_PC059IONode_getClockFrequenciesTable_overloads())
      .def("getStatus", &pdt::PC059IONode::getStatus, pdt_PC059IONode_getStatus_overloads())
      .def("getPLLStatus", &pdt::PC059IONode::getPLLStatus, pdt_TLUIONode_getPLLStatus_overloads())
      .def("getHardwareInfo", &pdt::PC059IONode::getHardwareInfo, pdt_PC059IONode_getHardwareInfo_overloads())
      .def("getSFPStatus", &pdt::PC059IONode::getSFPStatus, pdt_PC059IONode_getSFPStatus_overloads())
      .def("switchSFPSoftTxControlBit", &pdt::PC059IONode::switchSFPSoftTxControlBit)
      .def("switchSFPMUXChannel", &pdt::PC059IONode::switchSFPMUXChannel)
      .def("readActiveSFPMUXChannel", &pdt::PC059IONode::readActiveSFPMUXChannel)
      ;

      class_<pdt::TLUIONode, bases<uhal::Node, pdt::IONode> > ("TLUIONode", init<const uhal::Node&>())
      .def("reset", &pdt::TLUIONode::reset, pdt_TLUIONode_reset_overloads())
      .def("softReset", &pdt::TLUIONode::softReset)
      .def("getClockFrequenciesTable", &pdt::TLUIONode::getClockFrequenciesTable, pdt_TLUIONode_getClockFrequenciesTable_overloads())
      .def("getStatus", &pdt::TLUIONode::getStatus, pdt_TLUIONode_getStatus_overloads())
      .def("getPLLStatus", &pdt::TLUIONode::getPLLStatus, pdt_TLUIONode_getPLLStatus_overloads())
      .def("getHardwareInfo", &pdt::TLUIONode::getHardwareInfo, pdt_TLUIONode_getHardwareInfo_overloads())
      .def("getSFPStatus", &pdt::TLUIONode::getSFPStatus, pdt_TLUIONode_getSFPStatus_overloads())
      .def("switchSFPSoftTxControlBit", &pdt::TLUIONode::switchSFPSoftTxControlBit)
      .def("configureDAC", &pdt::TLUIONode::configureDAC, pdt_TLUIONode_configureDAC_overloads())
      ;


      class_<pdt::SIMIONode, bases<uhal::Node, pdt::IONode> > ("SIMIONode", init<const uhal::Node&>())
      .def("reset", &pdt::SIMIONode::reset, pdt_SIMIONode_reset_overloads())
      .def("softReset", &pdt::SIMIONode::softReset)
      .def("getClockFrequenciesTable", &pdt::SIMIONode::getClockFrequenciesTable, pdt_SIMIONode_getClockFrequenciesTable_overloads())
      .def("getStatus", &pdt::SIMIONode::getStatus, pdt_SIMIONode_getStatus_overloads())
      .def("getPLLStatus", &pdt::SIMIONode::getPLLStatus, pdt_SIMIONode_getPLLStatus_overloads())
      .def("getHardwareInfo", &pdt::SIMIONode::getHardwareInfo, pdt_SIMIONode_getHardwareInfo_overloads())
      .def("getSFPStatus", &pdt::SIMIONode::getSFPStatus, pdt_SIMIONode_getSFPStatus_overloads())
      .def("switchSFPSoftTxControlBit", &pdt::SIMIONode::switchSFPSoftTxControlBit)
      ;

}

} // namespace python
} // namespace pdt