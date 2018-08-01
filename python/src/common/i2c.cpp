// #include "pdt/python/registrators.hpp"


// Boost Headers
#include <boost/python/def.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/class.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/operators.hpp>

#include "pdt/I2CBaseNode.hpp"
#include "pdt/I2CMasterNode.hpp"
#include "pdt/I2CSlave.hpp"
#include "pdt/SIChipSlave.hpp"
// #include "pdt/MiniPODMasterNode.hpp"
#include "pdt/SI5344Node.hpp"
#include "pdt/SI534xNode.hpp"
#include "pdt/I2CExpanderNode.hpp"

// Namespace resolution
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CBaseNode_writeI2C_overloads, writeI2C, 3, 4);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CBaseNode_writeI2CArray_overloads, writeI2CArray, 3, 4);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CBaseNode_writeI2CPrimitive_overloads, writeI2CPrimitive, 2, 3);

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CSlave_writeI2C_overloads, writeI2C, 2, 3);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CSlave_writeI2CArray_overloads, writeI2CArray, 2, 3);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CSlave_writeI2CPrimitive_overloads, writeI2CPrimitive, 1, 2);


namespace pdt {
namespace python {

void 
register_i2c() {
        // .def("hardReset", (void ( mp7::CtrlNode::*) (double)) 0, mp7_CTRLNODE_hardReset_overloads())

  // Wrap pdt::I2CMasterNode
  class_<pdt::I2CBaseNode, bases<uhal::Node> > ("I2CBaseNode", init<const uhal::Node&>())
      .def("getI2CClockPrescale", &pdt::I2CBaseNode::getI2CClockPrescale)
      .def("readI2C", &pdt::I2CBaseNode::readI2C)
      .def("writeI2C", &pdt::I2CBaseNode::writeI2C, pdt_I2CBaseNode_writeI2C_overloads())
      .def("readI2CArray", &pdt::I2CBaseNode::readI2CArray)
      .def("writeI2CArray", &pdt::I2CBaseNode::writeI2CArray, pdt_I2CBaseNode_writeI2CArray_overloads())
      .def("readI2CPrimitive", &pdt::I2CBaseNode::readI2CPrimitive)
      .def("writeI2CPrimitive", &pdt::I2CBaseNode::writeI2CPrimitive, pdt_I2CBaseNode_writeI2CPrimitive_overloads())
      .def("getSlaves", &pdt::I2CBaseNode::getSlaves)
      .def("getSlaveAddress", &pdt::I2CBaseNode::getSlaveAddress)
      ;

  // Wrap pdt::I2CMasterNode
  class_<pdt::I2CMasterNode, bases<pdt::I2CBaseNode> > ("I2CMasterNode", init<const uhal::Node&>())
      .def("getSlave", &pdt::I2CMasterNode::getSlave, return_internal_reference<>())
      ;

  // Wrap pdt::I2CSlave    
  class_<pdt::I2CSlave, boost::noncopyable>("I2CSlave", no_init)
      .def("getI2CAddress", &pdt::I2CSlave::getI2CAddress)
      .def("readI2C", &pdt::I2CSlave::readI2C)
      .def("writeI2C", &pdt::I2CSlave::writeI2C, pdt_I2CSlave_writeI2C_overloads())
      .def("readI2CArray", &pdt::I2CSlave::readI2CArray)
      .def("writeI2CArray", &pdt::I2CSlave::writeI2CArray, pdt_I2CSlave_writeI2CArray_overloads())
      .def("readI2CPrimitive", &pdt::I2CSlave::readI2CPrimitive)
      .def("writeI2CPrimitive", &pdt::I2CSlave::writeI2CPrimitive, pdt_I2CSlave_writeI2CPrimitive_overloads())
      ;

  // Wrap SIChipSlave
  class_<pdt::SIChipSlave, bases<pdt::I2CSlave>, boost::noncopyable > ("SIChipSlave", init<const pdt::I2CBaseNode*, uint8_t>())
      .def("readPage", &pdt::SIChipSlave::readPage)
      .def("switchPage", &pdt::SIChipSlave::switchPage)
      .def("readDeviceVersion", &pdt::SIChipSlave::readDeviceVersion)
      .def("readClockRegister", &pdt::SIChipSlave::readClockRegister)
      .def("writeClockRegister", &pdt::SIChipSlave::writeClockRegister)
      ;

  // Wrap SI5344Slave
  class_<pdt::SI5344Slave, bases<pdt::SIChipSlave>, boost::noncopyable > ("SI5344Slave", init<const pdt::I2CBaseNode*, uint8_t>())
      // .def("readPage", &pdt::SI5344Slave::readPage)
      // .def("switchPage", &pdt::SI5344Slave::switchPage)
      // .def("readDeviceVersion", &pdt::SI5344Slave::readDeviceVersion)
      // .def("readClockRegister", &pdt::SI5344Slave::readClockRegister)
      // .def("writeClockRegister", &pdt::SI5344Slave::writeClockRegister)
      .def("configure", &pdt::SI5344Slave::configure)
      .def("readConfigID", &pdt::SI5344Slave::readConfigID)
      ;

  // Wrap SI5344Node
  class_<pdt::SI5344Node, bases<pdt::SI5344Slave, pdt::I2CBaseNode> > ("SI5344Node", init<const uhal::Node&>())
      ;


  // Wrap SI534xSlave
  class_<pdt::SI534xSlave, bases<pdt::SIChipSlave>, boost::noncopyable > ("SI534xSlave", init<const pdt::I2CBaseNode*, uint8_t>())
      .def("configure", &pdt::SI534xSlave::configure)
      .def("readConfigID", &pdt::SI534xSlave::readConfigID)
      // .def("registers", &pdt::SI534xSlave::registers)
      ;

  // Wrap SI534xNode
  class_<pdt::SI534xNode, bases<pdt::SI534xSlave, pdt::I2CBaseNode> > ("SI534xNode", init<const uhal::Node&>())
      ;

  // Wrap I2CExpanderSlave
  class_<pdt::I2CExpanderSlave, bases<pdt::I2CSlave>, boost::noncopyable > ("I2CExpanderSlave", init<const pdt::I2CBaseNode*, uint8_t>())
      // .def("enable", &pdt::I2CExpanderSlave::enable)
      .def("setIO", &pdt::I2CExpanderSlave::setIO)
      .def("setInversion", &pdt::I2CExpanderSlave::setInversion)
      .def("setOutputs", &pdt::I2CExpanderSlave::setOutputs)
      .def("readInputs", &pdt::I2CExpanderSlave::readInputs)
      .def("debug", &pdt::I2CExpanderSlave::debug)
      ;

  // Wrap I2CExpanderNode
  class_<pdt::I2CExpanderNode, bases<pdt::I2CExpanderSlave, pdt::I2CBaseNode> > ("I2CExpanderNode", init<const uhal::Node&>())
      ;

}

} // namespace python
} // namespace pdt
