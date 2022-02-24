/**
 * @file i2c.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/I2CMasterNode.hpp"
#include "timing/I2CSlave.hpp"
#include "timing/SIChipSlave.hpp"
// #include "timing/MiniPODMasterNode.hpp"
#include "timing/DACNode.hpp"
#include "timing/I2CExpanderNode.hpp"
#include "timing/SI534xNode.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

void
register_i2c(py::module& m)
{
  // .def("hardReset", (void ( mp7::CtrlNode::*) (double)) 0, mp7_CTRLNODE_hardReset_overloads())

  // Wrap timing::I2CMasterNode
  py::class_<timing::I2CMasterNode, uhal::Node>(m, "I2CMasterNode")
    .def(py::init<const uhal::Node&>())
    .def("get_i2c_clock_prescale", &timing::I2CMasterNode::get_i2c_clock_prescale)
    .def("read_i2c", &timing::I2CMasterNode::read_i2c)
    .def("write_i2c",
         &timing::I2CMasterNode::write_i2c,
         py::arg("i2c_device_address"),
         py::arg("i2c_reg_address"),
         py::arg("data"),
         py::arg("send_stop") = true)
    .def("read_i2cArray", &timing::I2CMasterNode::read_i2cArray)
    .def("write_i2cArray",
         &timing::I2CMasterNode::write_i2cArray,
         py::arg("i2c_device_address"),
         py::arg("i2c_reg_address"),
         py::arg("data"),
         py::arg("send_stop") = true)
    .def("read_i2cPrimitive", &timing::I2CMasterNode::read_i2cPrimitive)
    .def("write_i2cPrimitive",
         &timing::I2CMasterNode::write_i2cPrimitive,
         py::arg("i2c_device_address"),
         py::arg("data"),
         py::arg("send_stop") = true)
    .def("get_slaves", &timing::I2CMasterNode::get_slaves)
    .def("get_slave", &timing::I2CMasterNode::get_slave, py::return_value_policy::reference_internal)
    .def("get_slave_address", &timing::I2CMasterNode::get_slave_address)
    .def("ping", &timing::I2CMasterNode::ping)
    .def("scan", &timing::I2CMasterNode::scan)
    .def("reset", &timing::I2CMasterNode::reset);

  // Wrap timing::I2CSlave
  py::class_<timing::I2CSlave>(m, "I2CSlave")
    .def("get_i2c_address", &timing::I2CSlave::get_i2c_address)
    .def<uint8_t (timing::I2CSlave::*)(uint32_t) const>("read_i2c", // NOLINT(build/unsigned)
                                                        &timing::I2CSlave::read_i2c)
    .def<uint8_t (timing::I2CSlave::*)(uint32_t, uint32_t) const>("read_i2c", // NOLINT(build/unsigned)
                                                                  &timing::I2CSlave::read_i2c)
    .def<void (timing::I2CSlave::*)(uint32_t, uint8_t, bool) const>("write_i2c", // NOLINT(build/unsigned)
                                                                    &timing::I2CSlave::write_i2c,
                                                                    py::arg("i2c_reg_address"),
                                                                    py::arg("data"),
                                                                    py::arg("send_stop") = true)
    .def<void (timing::I2CSlave::*)(uint32_t, uint32_t, uint8_t, bool) const>("write_i2c", // NOLINT(build/unsigned)
                                                                              &timing::I2CSlave::write_i2c,
                                                                              py::arg("i2c_device_address"),
                                                                              py::arg("i2c_reg_address"),
                                                                              py::arg("data"),
                                                                              py::arg("send_stop") = true)
    .def<std::vector<uint8_t> (timing::I2CSlave::*)(uint32_t, uint32_t) const>( // NOLINT(build/unsigned)
      "read_i2cArray",
      &timing::I2CSlave::read_i2cArray)
    .def<std::vector<uint8_t> (timing::I2CSlave::*)(uint32_t, uint32_t, uint32_t) const>( // NOLINT(build/unsigned)
      "read_i2cArray",
      &timing::I2CSlave::read_i2cArray)
    .def<void (timing::I2CSlave::*)(uint32_t, std::vector<uint8_t>, bool) const>( // NOLINT(build/unsigned)
      "write_i2cArray",
      &timing::I2CSlave::write_i2cArray,
      py::arg("i2c_reg_address"),
      py::arg("data"),
      py::arg("send_stop") = true)
    .def<void (timing::I2CSlave::*)(uint32_t, uint32_t, std::vector<uint8_t>, bool) const>( // NOLINT(build/unsigned)
      "write_i2cArray",
      &timing::I2CSlave::write_i2cArray,
      py::arg("i2c_device_address"),
      py::arg("i2c_reg_address"),
      py::arg("data"),
      py::arg("send_stop") = true)
    .def("read_i2cPrimitive", &timing::I2CSlave::read_i2cPrimitive)
    .def("write_i2cPrimitive", &timing::I2CSlave::write_i2cPrimitive, py::arg("data"), py::arg("send_stop") = true)
    .def("ping", &timing::I2CSlave::ping);

  // Wrap SIChipSlave
  py::class_<timing::SIChipSlave, timing::I2CSlave>(m, "SIChipSlave")
    .def(py::init<const timing::I2CMasterNode*, uint8_t>()) // NOLINT(build/unsigned)
    .def("read_page", &timing::SIChipSlave::read_page)
    .def("switch_page", &timing::SIChipSlave::switch_page)
    .def("read_device_version", &timing::SIChipSlave::read_device_version)
    .def("read_clock_register", &timing::SIChipSlave::read_clock_register)
    .def("write_clock_register", &timing::SIChipSlave::write_clock_register);

  // Wrap SI534xSlave
  py::class_<timing::SI534xSlave, timing::SIChipSlave>(m, "SI534xSlave")
    .def(py::init<const timing::I2CMasterNode*, uint8_t>()) // NOLINT(build/unsigned)
    .def("configure", &timing::SI534xSlave::configure)
    .def("read_config_id", &timing::SI534xSlave::read_config_id)
    // .def("registers", &timing::SI534xSlave::registers)
    ;

  // Wrap SI534xNode
  py::class_<timing::SI534xNode, timing::SI534xSlave, timing::I2CMasterNode>(m, "SI534xNode")
    .def(py::init<const uhal::Node&>());

  // Wrap I2CExpanderSlave
  py::class_<timing::I2CExpanderSlave, timing::I2CSlave>(m, "I2CExpanderSlave")
    .def(py::init<const timing::I2CMasterNode*, uint8_t>()) // NOLINT(build/unsigned)
    .def("set_io", &timing::I2CExpanderSlave::set_io)
    .def("set_inversion", &timing::I2CExpanderSlave::set_inversion)
    .def("set_outputs", &timing::I2CExpanderSlave::set_outputs)
    .def("read_inputs", &timing::I2CExpanderSlave::read_inputs)
    .def("debug", &timing::I2CExpanderSlave::debug);

//  // Wrap I2CExpanderNode
//  py::class_<timing::I2CExpanderNode, timing::I2CExpanderSlave, timing::I2CMasterNode>(m, "I2CExpanderNode")
//    .def(py::init<const uhal::Node&>());

  // Wrap DACSlave
  py::class_<timing::DACSlave, timing::I2CSlave>(m, "DACSlave")
    .def(py::init<const timing::I2CMasterNode*, uint8_t>()) // NOLINT(build/unsigned)
    .def("set_interal_ref", &timing::DACSlave::set_interal_ref)
    .def("set_dac", &timing::DACSlave::set_dac);

  // Wrap DACNode
  py::class_<timing::DACNode, timing::DACSlave, timing::I2CMasterNode>(m, "DACNode").def(py::init<const uhal::Node&>());
} // NOLINT(readability/fn_size)

} // namespace python
} // namespace timing
} // namespace dunedaq