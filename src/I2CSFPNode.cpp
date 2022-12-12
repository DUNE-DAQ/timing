/**
 * @file I2CSFPNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/I2CSFPNode.hpp"

#include "logging/Logging.hpp"

#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
I2CSFPSlave::I2CSFPSlave(const I2CMasterNode* i2c_master, uint8_t address) // NOLINT(build/unsigned)
  : I2CSlave(i2c_master, address)
  , m_calibration_parameter_start_addresses({ 0x4C, 0x50, 0x54, 0x58 }) // laser current, tx_pwr, temp, voltage
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
I2CSFPSlave::~I2CSFPSlave() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CSFPSlave::sfp_reachable() const
{
  if (!this->ping()) {
    throw SFPUnreachable(ERS_HERE, get_master_id());
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CSFPSlave::ddm_available() const
{
  if (!read_ddm_support_bit()) {
    throw SFPDDMUnsupported(ERS_HERE, get_master_id());
  } else {
    if (read_i2c_reg_addressSwapBit()) {
      throw SFPDDMI2CAddressSwapUnsupported(ERS_HERE, get_master_id());
    }
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::pair<double, double>
I2CSFPSlave::read_calibration_parameter_pair(uint32_t calib_parameter_id) const // NOLINT(build/unsigned)
{

  ddm_available();

  auto parameter_array = this->read_i2cArray(0x51, m_calibration_parameter_start_addresses.at(calib_parameter_id), 0x4);

  // slope
  double slope = parameter_array.at(0) + (parameter_array.at(1) / 256.0);

  uint32_t offset_raw = (parameter_array.at(2) & 0x7f) << 8 | parameter_array.at(3); // NOLINT(build/unsigned)

  // eighth bit corresponds to sign
  double offset = parameter_array.at(2) & (1UL << 7) ? offset_raw - 0x8000 : offset_raw;

  return std::make_pair(slope, offset);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
I2CSFPSlave::read_temperature_raw() const
{
  ddm_available();
  auto temperature_array = this->read_i2cArray(0x51, 0x60, 0x2);

  // bit 7 corresponds to temperature sign, 0 for pos, 1 for neg
  double temperature =
    temperature_array.at(0) & (1UL << 7) ? (temperature_array.at(0) & 0x7f) - 0xff : temperature_array.at(0);
  return temperature + (temperature_array.at(1) / 256.0);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
I2CSFPSlave::read_temperature() const
{
  auto temperature_raw = read_temperature_raw();
  auto temp_calib_pair = read_calibration_parameter_pair(0x2);
  return temperature_raw * temp_calib_pair.first + temp_calib_pair.second;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
I2CSFPSlave::read_voltage_raw() const
{
  ddm_available();
  auto voltage_array = this->read_i2cArray(0x51, 0x62, 0x2);
  return (voltage_array.at(0) << 8) | voltage_array.at(1);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
I2CSFPSlave::read_voltage() const
{
  auto voltage_raw = this->read_voltage_raw();
  auto temp_calib_pair = read_calibration_parameter_pair(0x3);
  return ((voltage_raw * temp_calib_pair.first) + temp_calib_pair.second) * 1e-4;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
I2CSFPSlave::read_rx_power_raw() const
{
  ddm_available();
  auto rx_power_array = this->read_i2cArray(0x51, 0x68, 0x2);
  return (rx_power_array.at(0) << 8) | rx_power_array.at(1);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
I2CSFPSlave::read_rx_ower() const
{
  auto rx_power_raw = this->read_rx_power_raw();
  // rx power calib constants, 5 4-byte parameters, IEEE 754 float encoding
  std::vector<uint32_t> rx_param_start_adr = { 0x48, 0x44, 0x40, 0x3C, 0x38 }; // NOLINT(build/unsigned)
  std::vector<double> rx_parameters;
  for (auto it = rx_param_start_adr.begin(); it != rx_param_start_adr.end(); ++it) {
    uint32_t parameter_bits = 0; // NOLINT(build/unsigned)
    auto parameter_array = this->read_i2cArray(0x51, *it, 0x4);

    for (auto jt = parameter_array.begin(); jt != parameter_array.end(); ++jt)
      parameter_bits = (parameter_bits << 8) | *jt;

    // convert the 32 bits to a float according IEEE 754
    double parameter = convert_bits_to_float(parameter_bits);
    rx_parameters.push_back(parameter);
  }

  double rx_power_calib = 0;
  for (uint32_t i = 0; i < rx_parameters.size(); ++i) { // NOLINT(build/unsigned)
    double parameter = rx_parameters.at(i);
    rx_power_calib = rx_power_calib + (parameter * pow(rx_power_raw, i));
  }
  return rx_power_calib * 0.1;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
I2CSFPSlave::read_tx_power_raw() const
{
  ddm_available();
  auto tx_power_array = this->read_i2cArray(0x51, 0x66, 0x2);
  return (tx_power_array.at(0) << 8) | tx_power_array.at(1);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
I2CSFPSlave::read_tx_power() const
{
  auto tx_power_raw = this->read_tx_power_raw();
  auto temp_calib_pair = read_calibration_parameter_pair(0x1);
  return ((tx_power_raw * temp_calib_pair.first) + temp_calib_pair.second) * 0.1;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
I2CSFPSlave::read_current_raw() const
{
  ddm_available();
  auto current_array = this->read_i2cArray(0x51, 0x64, 0x2);
  return (current_array.at(0) << 8) | current_array.at(1);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
I2CSFPSlave::read_current() const
{
  auto current_raw = this->read_current_raw();
  auto temp_calib_pair = read_calibration_parameter_pair(0x0);
  return ((current_raw * temp_calib_pair.first) + temp_calib_pair.second) * 0.002;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
I2CSFPSlave::read_vendor_name() const
{
  sfp_reachable();

  std::stringstream vendor_name;
  auto vendor_name_characters = this->read_i2cArray(0x14, 0x10);
  for (auto it = vendor_name_characters.begin(); it != vendor_name_characters.end(); ++it) {
    vendor_name << *it;
  }
  return vendor_name.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
I2CSFPSlave::read_vendor_part_number() const
{
  sfp_reachable();

  std::stringstream vendor_pn;
  auto vendor_pn_characters = this->read_i2cArray(0x28, 0x10);
  for (auto it = vendor_pn_characters.begin(); it != vendor_pn_characters.end(); ++it) {
    vendor_pn << *it;
  }
  return vendor_pn.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
I2CSFPSlave::read_serial_number() const
{
  sfp_reachable();

  std::stringstream serial_number;
  auto serial_number_characters = this->read_i2cArray(0x44, 0x10);
  for (auto it = serial_number_characters.begin(); it != serial_number_characters.end(); ++it) {
    serial_number << *it;
  }
  return serial_number.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
I2CSFPSlave::read_ddm_support_bit() const
{
  sfp_reachable();

  // Bit 6 of reg 5C tells us whether the SFP supports digital diagnostic monitoring (DDM)
  auto ddm_info_byte = this->read_i2c(0x5C);
  return ddm_info_byte & 0x40;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
I2CSFPSlave::read_soft_tx_control_support_bit() const
{
  sfp_reachable();

  // Bit 6 of reg 5d tells us whether the soft tx control is implemented in this sfp
  auto enhanced_options_byte = this->read_i2c(0x5d);
  return enhanced_options_byte & 0x40;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
I2CSFPSlave::read_soft_tx_control_state() const
{
  ddm_available();
  auto opt_status_ctrl_byte = this->read_i2c(0x51, 0x6e);
  // Bit 6 tells us the state of the soft tx_disble register
  return opt_status_ctrl_byte & 0x40;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
I2CSFPSlave::read_tx_disable_pin_state() const
{
  ddm_available();
  auto opt_status_ctrl_byte = this->read_i2c(0x51, 0x6e);
  // Bit 7 tells us the state of tx_disble pin
  return opt_status_ctrl_byte & 0x80;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
I2CSFPSlave::read_i2c_reg_addressSwapBit() const
{
  sfp_reachable();

  auto ddm_info_byte = this->read_i2c(0x5C);
  // Bit 2 of byte 5C tells us whether special I2C address change operations are needed to access the DDM area
  return ddm_info_byte & 0x4;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CSFPSlave::switch_soft_tx_control_bit(bool turn_on) const
{
  ddm_available();

  if (!read_soft_tx_control_support_bit()) {
    throw SoftTxLaserControlUnsupported(ERS_HERE, get_master_id());
  }

  // Get optional status/control bits
  auto opt_status_ctrl_byte = this->read_i2c(0x51, 0x6e);

  uint8_t new_opt_status_ctrl_byte; // NOLINT(build/unsigned)
  // Bit 6 of byte 0x6e controls the soft tx_disable
  if (turn_on) {
    new_opt_status_ctrl_byte = opt_status_ctrl_byte & ~(1UL << 6);
  } else {
    new_opt_status_ctrl_byte = opt_status_ctrl_byte | (1UL << 6);
  }
  this->write_i2c(0x51, 0x6e, new_opt_status_ctrl_byte);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
I2CSFPSlave::get_status(bool print_out) const
{
  sfp_reachable();

  std::stringstream status;
  std::vector<std::pair<std::string, std::string>> sfp_info;

  // Vendor name
  sfp_info.push_back(std::make_pair("Vendor", read_vendor_name()));

  // Vendor part number
  sfp_info.push_back(std::make_pair("Part number", read_vendor_part_number()));

  // Serial number
  sfp_info.push_back(std::make_pair("Serial number", read_serial_number()));

  // Does the SFP support DDM
  if (!read_ddm_support_bit()) {
    TLOG() << "DDM not available for SFP on I2C bus: " << get_master_id();
    status << format_reg_table(sfp_info, "SFP status", { "", "" });
    if (print_out)
      TLOG() << status.str();
    return status.str();
  } else {
    if (read_i2c_reg_addressSwapBit()) {
      TLOG() << "SFP DDM I2C address swap not supported. SFP on I2C bus: " << get_master_id();
      status << format_reg_table(sfp_info, "SFP status", { "", "" });
      if (print_out)
        TLOG() << status.str();
      return status.str();
    }
  }

  std::stringstream temperature_stream;
  temperature_stream << std::dec << std::fixed << std::setprecision(2) << read_temperature() << " C";
  sfp_info.push_back(std::make_pair("Temperature", temperature_stream.str()));

  std::stringstream voltage_stream;
  voltage_stream << std::dec << std::fixed << std::setprecision(2) << read_voltage() << " V";
  sfp_info.push_back(std::make_pair("Supply voltage", voltage_stream.str()));

  std::stringstream rx_power_stream;
  rx_power_stream << std::dec << std::fixed << std::setprecision(2) << read_rx_ower() << " uW";
  sfp_info.push_back(std::make_pair("Rx power", rx_power_stream.str()));

  std::stringstream tx_power_stream;
  tx_power_stream << std::dec << std::fixed << std::setprecision(2) << read_tx_power() << " uW";
  sfp_info.push_back(std::make_pair("Tx power", tx_power_stream.str()));

  std::stringstream current_stream;
  current_stream << std::dec << std::fixed << std::setprecision(2) << read_current() << " uA";
  sfp_info.push_back(std::make_pair("Tx current", current_stream.str()));

  if (read_soft_tx_control_support_bit()) {
    // sfp_info.push_back(std::make_pair("Soft Tx disbale supported",  "True"));
    sfp_info.push_back(std::make_pair("Tx disable bit", std::to_string(read_soft_tx_control_state())));
  } else {
    sfp_info.push_back(std::make_pair("Soft Tx disbale supported", "False"));
  }

  sfp_info.push_back(std::make_pair("Tx disable pin", std::to_string(read_tx_disable_pin_state())));

  status << format_reg_table(sfp_info, "SFP status", { "", "" });
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CSFPSlave::get_info(timinghardwareinfo::TimingSFPMonitorData& mon_data) const
{
  mon_data.data_valid = false;

  sfp_reachable();

  // Vendor name
  mon_data.vendor_name = this->read_vendor_name();

  // Vendor part number
  mon_data.vendor_pn = this->read_vendor_part_number();

  // Serial number TP DO?
  // sfp_info.push_back(std::make_pair("Serial number", read_serial_number()));

  // Does the SFP support DDM
  if (!this->read_ddm_support_bit()) {
    TLOG() << "DDM not available for SFP on I2C bus: " << get_master_id();
    mon_data.ddm_supported = false;
    return;
  } else {
    mon_data.ddm_supported = true;
    if (this->read_i2c_reg_addressSwapBit()) {
      TLOG() << "SFP DDM I2C address swap not supported. SFP on I2C bus: " << get_master_id();
      return;
    }
  }

  mon_data.temperature = this->read_temperature();

  mon_data.supply_voltage = this->read_voltage();

  mon_data.rx_power = this->read_rx_ower();

  mon_data.tx_power = this->read_tx_power();

  mon_data.laser_current = this->read_current();

  mon_data.tx_disable_sw_supported = this->read_soft_tx_control_support_bit();

  mon_data.tx_disable_sw = this->read_soft_tx_control_state();

  mon_data.tx_disable_hw = this->read_tx_disable_pin_state();

  mon_data.data_valid = true;
}

void
I2CSFPSlave::get_info(opmonlib::InfoCollector& ci, int /*level*/) const
{
  timinghardwareinfo::TimingSFPMonitorData sfp_mon_data;
  get_info(sfp_mon_data);
  ci.add(sfp_mon_data);
}
//-----------------------------------------------------------------------------

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(I2CSFPNode)

//-----------------------------------------------------------------------------
I2CSFPNode::I2CSFPNode(const uhal::Node& node)
  : I2CMasterNode(node)
  , I2CSFPSlave(this, this->get_slave_address("SFP_EEProm"))
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
I2CSFPNode::I2CSFPNode(const I2CSFPNode& node)
  : I2CMasterNode(node)
  , I2CSFPSlave(this, this->get_slave_address("SFP_EEProm"))
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
I2CSFPNode::~I2CSFPNode() {}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq