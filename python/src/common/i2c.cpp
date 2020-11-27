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

#include "pdt/I2CMasterNode.hpp"
#include "pdt/I2CMasterNode.hpp"
#include "pdt/I2CSlave.hpp"
#include "pdt/SIChipSlave.hpp"
// #include "pdt/MiniPODMasterNode.hpp"
#include "pdt/DACNode.hpp"
#include "pdt/SI534xNode.hpp"
#include "pdt/I2CExpanderNode.hpp"

// Namespace resolution
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CMasterNode_writeI2C_overloads, writeI2C, 3, 4);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CMasterNode_writeI2CArray_overloads, writeI2CArray, 3, 4);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CMasterNode_writeI2CPrimitive_overloads, writeI2CPrimitive, 2, 3);

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CSlave_writeI2C_overloads, writeI2C, 2, 3);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CSlave_writeI2C_aDeviceAddress_overloads, writeI2C, 3, 4);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CSlave_writeI2CArray_overloads, writeI2CArray, 2, 3);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CSlave_writeI2CArray_aDeviceAddress_overloads, writeI2CArray, 3, 4);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(pdt_I2CSlave_writeI2CPrimitive_overloads, writeI2CPrimitive, 1, 2);


namespace pdt {
namespace python {

void 
register_i2c() {
        // .def("hardReset", (void ( mp7::CtrlNode::*) (double)) 0, mp7_CTRLNODE_hardReset_overloads())

  // Wrap pdt::I2CMasterNode
  class_<pdt::I2CMasterNode, bases<uhal::Node> > ("I2CMasterNode", init<const uhal::Node&>())
      .def("getI2CClockPrescale", &pdt::I2CMasterNode::getI2CClockPrescale)
      .def("readI2C", &pdt::I2CMasterNode::readI2C)
      .def("writeI2C", &pdt::I2CMasterNode::writeI2C, pdt_I2CMasterNode_writeI2C_overloads())
      .def("readI2CArray", &pdt::I2CMasterNode::readI2CArray)
      .def("writeI2CArray", &pdt::I2CMasterNode::writeI2CArray, pdt_I2CMasterNode_writeI2CArray_overloads())
      .def("readI2CPrimitive", &pdt::I2CMasterNode::readI2CPrimitive)
      .def("writeI2CPrimitive", &pdt::I2CMasterNode::writeI2CPrimitive, pdt_I2CMasterNode_writeI2CPrimitive_overloads())
      .def("getSlaves", &pdt::I2CMasterNode::getSlaves)
      .def("getSlave", &pdt::I2CMasterNode::getSlave, return_internal_reference<>())
      .def("getSlaveAddress", &pdt::I2CMasterNode::getSlaveAddress)
      .def("ping", &pdt::I2CMasterNode::ping)
      .def("scan", &pdt::I2CMasterNode::scan)
      .def("reset", &pdt::I2CMasterNode::reset)
      ;


  // Wrap pdt::I2CSlave    
  class_<pdt::I2CSlave, boost::noncopyable>("I2CSlave", no_init)
      .def("getI2CAddress", &pdt::I2CSlave::getI2CAddress)
      .def<uint8_t (pdt::I2CSlave::*)(uint32_t) const>("readI2C", &pdt::I2CSlave::readI2C)
      .def<uint8_t (pdt::I2CSlave::*)(uint32_t,uint32_t) const>("readI2C", &pdt::I2CSlave::readI2C)
      .def<void (pdt::I2CSlave::*)(uint32_t,uint8_t,bool) const>("writeI2C", &pdt::I2CSlave::writeI2C, pdt_I2CSlave_writeI2C_overloads())
      .def<void (pdt::I2CSlave::*)(uint32_t,uint32_t,uint8_t,bool) const>("writeI2C", &pdt::I2CSlave::writeI2C, pdt_I2CSlave_writeI2C_aDeviceAddress_overloads())
      .def<std::vector<uint8_t> (pdt::I2CSlave::*)(uint32_t,uint32_t) const>("readI2CArray", &pdt::I2CSlave::readI2CArray)
      .def<std::vector<uint8_t> (pdt::I2CSlave::*)(uint32_t,uint32_t,uint32_t) const>("readI2CArray", &pdt::I2CSlave::readI2CArray)
      .def<void (pdt::I2CSlave::*)(uint32_t, std::vector<uint8_t>,bool) const>("writeI2CArray", &pdt::I2CSlave::writeI2CArray, pdt_I2CSlave_writeI2CArray_overloads())
      .def<void (pdt::I2CSlave::*)(uint32_t,uint32_t, std::vector<uint8_t>,bool) const>("writeI2CArray", &pdt::I2CSlave::writeI2CArray, pdt_I2CSlave_writeI2CArray_aDeviceAddress_overloads())
      .def("readI2CPrimitive", &pdt::I2CSlave::readI2CPrimitive)
      .def("writeI2CPrimitive", &pdt::I2CSlave::writeI2CPrimitive, pdt_I2CSlave_writeI2CPrimitive_overloads())
      .def("ping", &pdt::I2CSlave::ping)
      ;

  // Wrap SIChipSlave
  class_<pdt::SIChipSlave, bases<pdt::I2CSlave>, boost::noncopyable > ("SIChipSlave", init<const pdt::I2CMasterNode*, uint8_t>())
      .def("readPage", &pdt::SIChipSlave::readPage)
      .def("switchPage", &pdt::SIChipSlave::switchPage)
      .def("readDeviceVersion", &pdt::SIChipSlave::readDeviceVersion)
      .def("readClockRegister", &pdt::SIChipSlave::readClockRegister)
      .def("writeClockRegister", &pdt::SIChipSlave::writeClockRegister)
      ;

  // Wrap SI534xSlave
  class_<pdt::SI534xSlave, bases<pdt::SIChipSlave>, boost::noncopyable > ("SI534xSlave", init<const pdt::I2CMasterNode*, uint8_t>())
      .def("configure", &pdt::SI534xSlave::configure)
      .def("readConfigID", &pdt::SI534xSlave::readConfigID)
      // .def("registers", &pdt::SI534xSlave::registers)
      ;

  // Wrap SI534xNode
  class_<pdt::SI534xNode, bases<pdt::SI534xSlave, pdt::I2CMasterNode> > ("SI534xNode", init<const uhal::Node&>())
      ;

  // Wrap I2CExpanderSlave
  class_<pdt::I2CExpanderSlave, bases<pdt::I2CSlave>, boost::noncopyable > ("I2CExpanderSlave", init<const pdt::I2CMasterNode*, uint8_t>())
      .def("setIO", &pdt::I2CExpanderSlave::setIO)
      .def("setInversion", &pdt::I2CExpanderSlave::setInversion)
      .def("setOutputs", &pdt::I2CExpanderSlave::setOutputs)
      .def("readInputs", &pdt::I2CExpanderSlave::readInputs)
      .def("debug", &pdt::I2CExpanderSlave::debug)
      ;

  // Wrap I2CExpanderNode
  class_<pdt::I2CExpanderNode, bases<pdt::I2CExpanderSlave, pdt::I2CMasterNode> > ("I2CExpanderNode", init<const uhal::Node&>())
      ;

  // Wrap DACSlave
  class_<pdt::DACSlave, bases<pdt::I2CSlave>, boost::noncopyable > ("DACSlave", init<const pdt::I2CMasterNode*, uint8_t>())
      .def("setInteralRef", &pdt::DACSlave::setInteralRef)
      .def("setDAC", &pdt::DACSlave::setDAC)
      ;

  // Wrap DACNode
  class_<pdt::DACNode, bases<pdt::DACSlave, pdt::I2CMasterNode> > ("DACNode", init<const uhal::Node&>())
      ;

}

} // namespace python
} // namespace pdt
