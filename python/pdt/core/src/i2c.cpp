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
    .def("get_i2c_clock_prescale", &pdt::I2CMasterNode::get_i2c_clock_prescale)
    .def("read_i2c", &pdt::I2CMasterNode::read_i2c)
    .def("write_i2c", &pdt::I2CMasterNode::write_i2c, py::arg("i2c_device_address"), py::arg("i2c_reg_address"), py::arg("data"), py::arg("send_stop") = true)
    .def("read_i2cArray", &pdt::I2CMasterNode::read_i2cArray)
    .def("write_i2cArray", &pdt::I2CMasterNode::write_i2cArray, py::arg("i2c_device_address"), py::arg("i2c_reg_address"), py::arg("data"), py::arg("send_stop") = true)
    .def("read_i2cPrimitive", &pdt::I2CMasterNode::read_i2cPrimitive)
    .def("write_i2cPrimitive", &pdt::I2CMasterNode::write_i2cPrimitive, py::arg("i2c_device_address"), py::arg("data"), py::arg("send_stop") = true)
    .def("get_slaves", &pdt::I2CMasterNode::get_slaves)
    .def("get_slave", &pdt::I2CMasterNode::get_slave, py::return_value_policy::reference_internal)
    .def("get_slave_address", &pdt::I2CMasterNode::get_slave_address)
    .def("ping", &pdt::I2CMasterNode::ping)
    .def("scan", &pdt::I2CMasterNode::scan)
    .def("reset", &pdt::I2CMasterNode::reset)
    ;

  // Wrap pdt::I2CSlave    
  py::class_<pdt::I2CSlave>(m, "I2CSlave")
    .def("get_i2c_address", &pdt::I2CSlave::get_i2c_address)
    .def<uint8_t (pdt::I2CSlave::*)(uint32_t) const>("read_i2c", &pdt::I2CSlave::read_i2c)
    .def<uint8_t (pdt::I2CSlave::*)(uint32_t,uint32_t) const>("read_i2c", &pdt::I2CSlave::read_i2c)
    .def<void (pdt::I2CSlave::*)(uint32_t,uint8_t,bool) const>("write_i2c", &pdt::I2CSlave::write_i2c, py::arg("i2c_reg_address"), py::arg("data"), py::arg("send_stop") = true)
    .def<void (pdt::I2CSlave::*)(uint32_t,uint32_t,uint8_t,bool) const>("write_i2c", &pdt::I2CSlave::write_i2c, py::arg("i2c_device_address"), py::arg("i2c_reg_address"), py::arg("data"), py::arg("send_stop") = true)
    .def<std::vector<uint8_t> (pdt::I2CSlave::*)(uint32_t,uint32_t) const>("read_i2cArray", &pdt::I2CSlave::read_i2cArray)
    .def<std::vector<uint8_t> (pdt::I2CSlave::*)(uint32_t,uint32_t,uint32_t) const>("read_i2cArray", &pdt::I2CSlave::read_i2cArray)
    .def<void (pdt::I2CSlave::*)(uint32_t, std::vector<uint8_t>,bool) const>("write_i2cArray", &pdt::I2CSlave::write_i2cArray, py::arg("i2c_reg_address"), py::arg("data"), py::arg("send_stop") = true)
    .def<void (pdt::I2CSlave::*)(uint32_t,uint32_t, std::vector<uint8_t>,bool) const>("write_i2cArray", &pdt::I2CSlave::write_i2cArray, py::arg("i2c_device_address"), py::arg("i2c_reg_address"), py::arg("data"), py::arg("send_stop") = true)
    .def("read_i2cPrimitive", &pdt::I2CSlave::read_i2cPrimitive)
    .def("write_i2cPrimitive", &pdt::I2CSlave::write_i2cPrimitive, py::arg("data"), py::arg("send_stop") = true)
    .def("ping", &pdt::I2CSlave::ping)
    ;

  // Wrap SIChipSlave
  py::class_<pdt::SIChipSlave, pdt::I2CSlave> (m, "SIChipSlave")
    .def(py::init<const pdt::I2CMasterNode*, uint8_t>())
    .def("read_page", &pdt::SIChipSlave::read_page)
    .def("switch_page", &pdt::SIChipSlave::switch_page)
    .def("read_device_version", &pdt::SIChipSlave::read_device_version)
    .def("read_clock_register", &pdt::SIChipSlave::read_clock_register)
    .def("write_clock_register", &pdt::SIChipSlave::write_clock_register)
    ;

  // Wrap SI534xSlave
  py::class_<pdt::SI534xSlave, pdt::SIChipSlave> (m, "SI534xSlave")
    .def(py::init<const pdt::I2CMasterNode*, uint8_t>())
    .def("configure", &pdt::SI534xSlave::configure)
    .def("read_config_id", &pdt::SI534xSlave::read_config_id)
    // .def("registers", &pdt::SI534xSlave::registers)
    ;

  // Wrap SI534xNode
  py::class_<pdt::SI534xNode, pdt::SI534xSlave, pdt::I2CMasterNode > (m, "SI534xNode")
    .def(py::init<const uhal::Node&>())
    ;

  // Wrap I2CExpanderSlave
  py::class_<pdt::I2CExpanderSlave, pdt::I2CSlave> (m, "I2CExpanderSlave")
      .def(py::init<const pdt::I2CMasterNode*, uint8_t>())
      .def("set_io", &pdt::I2CExpanderSlave::set_io)
      .def("set_inversion", &pdt::I2CExpanderSlave::set_inversion)
      .def("set_outputs", &pdt::I2CExpanderSlave::set_outputs)
      .def("read_inputs", &pdt::I2CExpanderSlave::read_inputs)
      .def("debug", &pdt::I2CExpanderSlave::debug)
      ;

  // Wrap I2CExpanderNode
  py::class_<pdt::I2CExpanderNode, pdt::I2CExpanderSlave, pdt::I2CMasterNode> (m, "I2CExpanderNode")
    .def(py::init<const uhal::Node&>())
    ;

  // Wrap DACSlave
  py::class_<pdt::DACSlave, pdt::I2CSlave> (m, "DACSlave")
    .def(py::init<const pdt::I2CMasterNode*, uint8_t>())
    .def("set_interal_ref", &pdt::DACSlave::set_interal_ref)
    .def("set_dac", &pdt::DACSlave::set_dac)
    ;

  // Wrap DACNode
  py::class_<pdt::DACNode, pdt::DACSlave, pdt::I2CMasterNode> (m, "DACNode")
    .def(py::init<const uhal::Node&>())
    ;

}

} // namespace python
} // namespace pdt
} // namespace dunedaq