#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "pdt/I2CMasterNode.hpp"
#include "pdt/I2CMasterNode.hpp"
#include "pdt/I2CSlave.hpp"
#include "pdt/SIChipSlave.hpp"
// #include "pdt/MiniPODMasterNode.hpp"
#include "pdt/DACNode.hpp"
#include "pdt/SI534xNode.hpp"
#include "pdt/I2CExpanderNode.hpp"

namespace py = pybind11;

namespace dunedaq {
namespace pdt {
namespace python {

void 
register_i2c(py::module& m) {
        // .def("hardReset", (void ( mp7::CtrlNode::*) (double)) 0, mp7_CTRLNODE_hardReset_overloads())

  // Wrap pdt::I2CMasterNode
  py::class_<pdt::I2CMasterNode, uhal::Node> (m, "I2CMasterNode")
    .def(py::init<const uhal::Node&>())
    .def("getI2CClockPrescale", &pdt::I2CMasterNode::getI2CClockPrescale)
    .def("readI2C", &pdt::I2CMasterNode::readI2C)
    .def("writeI2C", &pdt::I2CMasterNode::writeI2C, py::arg("aSlaveAddress"), py::arg("i2cAddress"), py::arg("aData"), py::arg("aSendStop") = true)
    .def("readI2CArray", &pdt::I2CMasterNode::readI2CArray)
    .def("writeI2CArray", &pdt::I2CMasterNode::writeI2CArray, py::arg("aSlaveAddress"), py::arg("i2cAddress"), py::arg("aData"), py::arg("aSendStop") = true)
    .def("readI2CPrimitive", &pdt::I2CMasterNode::readI2CPrimitive)
    .def("writeI2CPrimitive", &pdt::I2CMasterNode::writeI2CPrimitive, py::arg("aSlaveAddress"), py::arg("aData"), py::arg("aSendStop") = true)
    .def("getSlaves", &pdt::I2CMasterNode::getSlaves)
    .def("getSlave", &pdt::I2CMasterNode::getSlave, py::return_value_policy::reference_internal)
    .def("getSlaveAddress", &pdt::I2CMasterNode::getSlaveAddress)
    .def("ping", &pdt::I2CMasterNode::ping)
    .def("scan", &pdt::I2CMasterNode::scan)
    .def("reset", &pdt::I2CMasterNode::reset)
    ;

  // Wrap pdt::I2CSlave    
  py::class_<pdt::I2CSlave>(m, "I2CSlave")
    .def("getI2CAddress", &pdt::I2CSlave::getI2CAddress)
    .def<uint8_t (pdt::I2CSlave::*)(uint32_t) const>("readI2C", &pdt::I2CSlave::readI2C)
    .def<uint8_t (pdt::I2CSlave::*)(uint32_t,uint32_t) const>("readI2C", &pdt::I2CSlave::readI2C)
    .def<void (pdt::I2CSlave::*)(uint32_t,uint8_t,bool) const>("writeI2C", &pdt::I2CSlave::writeI2C, py::arg("i2cAddress"), py::arg("aData"), py::arg("aSendStop") = true)
    .def<void (pdt::I2CSlave::*)(uint32_t,uint32_t,uint8_t,bool) const>("writeI2C", &pdt::I2CSlave::writeI2C, py::arg("aSlaveAddress"), py::arg("i2cAddress"), py::arg("aData"), py::arg("aSendStop") = true)
    .def<std::vector<uint8_t> (pdt::I2CSlave::*)(uint32_t,uint32_t) const>("readI2CArray", &pdt::I2CSlave::readI2CArray)
    .def<std::vector<uint8_t> (pdt::I2CSlave::*)(uint32_t,uint32_t,uint32_t) const>("readI2CArray", &pdt::I2CSlave::readI2CArray)
    .def<void (pdt::I2CSlave::*)(uint32_t, std::vector<uint8_t>,bool) const>("writeI2CArray", &pdt::I2CSlave::writeI2CArray, py::arg("i2cAddress"), py::arg("aData"), py::arg("aSendStop") = true)
    .def<void (pdt::I2CSlave::*)(uint32_t,uint32_t, std::vector<uint8_t>,bool) const>("writeI2CArray", &pdt::I2CSlave::writeI2CArray, py::arg("aSlaveAddress"), py::arg("i2cAddress"), py::arg("aData"), py::arg("aSendStop") = true)
    .def("readI2CPrimitive", &pdt::I2CSlave::readI2CPrimitive)
    .def("writeI2CPrimitive", &pdt::I2CSlave::writeI2CPrimitive, py::arg("aData"), py::arg("aSendStop") = true)
    .def("ping", &pdt::I2CSlave::ping)
    ;

  // Wrap SIChipSlave
  py::class_<pdt::SIChipSlave, pdt::I2CSlave> (m, "SIChipSlave")
    .def(py::init<const pdt::I2CMasterNode*, uint8_t>())
    .def("readPage", &pdt::SIChipSlave::readPage)
    .def("switchPage", &pdt::SIChipSlave::switchPage)
    .def("readDeviceVersion", &pdt::SIChipSlave::readDeviceVersion)
    .def("readClockRegister", &pdt::SIChipSlave::readClockRegister)
    .def("writeClockRegister", &pdt::SIChipSlave::writeClockRegister)
    ;

  // Wrap SI534xSlave
  py::class_<pdt::SI534xSlave, pdt::SIChipSlave> (m, "SI534xSlave")
    .def(py::init<const pdt::I2CMasterNode*, uint8_t>())
    .def("configure", &pdt::SI534xSlave::configure)
    .def("readConfigID", &pdt::SI534xSlave::readConfigID)
    // .def("registers", &pdt::SI534xSlave::registers)
    ;

  // Wrap SI534xNode
  py::class_<pdt::SI534xNode, pdt::SI534xSlave, pdt::I2CMasterNode > (m, "SI534xNode")
    .def(py::init<const uhal::Node&>())
    ;

  // Wrap I2CExpanderSlave
  py::class_<pdt::I2CExpanderSlave, pdt::I2CSlave> (m, "I2CExpanderSlave")
      .def(py::init<const pdt::I2CMasterNode*, uint8_t>())
      .def("setIO", &pdt::I2CExpanderSlave::setIO)
      .def("setInversion", &pdt::I2CExpanderSlave::setInversion)
      .def("setOutputs", &pdt::I2CExpanderSlave::setOutputs)
      .def("readInputs", &pdt::I2CExpanderSlave::readInputs)
      .def("debug", &pdt::I2CExpanderSlave::debug)
      ;

  // Wrap I2CExpanderNode
  py::class_<pdt::I2CExpanderNode, pdt::I2CExpanderSlave, pdt::I2CMasterNode> (m, "I2CExpanderNode")
    .def(py::init<const uhal::Node&>())
    ;

  // Wrap DACSlave
  py::class_<pdt::DACSlave, pdt::I2CSlave> (m, "DACSlave")
    .def(py::init<const pdt::I2CMasterNode*, uint8_t>())
    .def("setInteralRef", &pdt::DACSlave::setInteralRef)
    .def("setDAC", &pdt::DACSlave::setDAC)
    ;

  // Wrap DACNode
  py::class_<pdt::DACNode, pdt::DACSlave, pdt::I2CMasterNode> (m, "DACNode")
    .def(py::init<const uhal::Node&>())
    ;

}

} // namespace python
} // namespace pdt
} // namespace dunedaq