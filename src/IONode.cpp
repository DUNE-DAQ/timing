/**
 * @file IONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/IONode.hpp"

#include "logging/Logging.hpp"

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace timing {

// UHAL_REGISTER_DERIVED_NODE(IONode);

//-----------------------------------------------------------------------------
IONode::IONode(const uhal::Node& node,
               std::string uid_i2c_bus,
               std::string uid_i2c_device,
               std::string pll_i2c_bus,
               std::string pll_i2c_device,
               std::vector<std::string> clock_names,
               std::vector<std::string> sfp_i2c_buses)
  : TimingNode(node)
  , m_uid_i2c_bus(uid_i2c_bus)
  , m_uid_i2c_device(uid_i2c_device)
  , m_pll_i2c_bus(pll_i2c_bus)
  , m_pll_i2c_device(pll_i2c_device)
  , m_clock_names(clock_names)
  , m_sfp_i2c_buses(sfp_i2c_buses)
// mPLL (new SI534xSlave( getNode<I2CMasterNode>(m_pll_i2c_bus)& ,
// getNode<I2CMasterNode>(m_pll_i2c_bus).get_slave_address(pll_i2c_device) ))
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
IONode::~IONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
IONode::read_board_type() const
{
  uhal::ValWord<uint32_t> board_type = getNode("config.board_type").read(); // NOLINT(build/unsigned)
  getClient().dispatch();
  return board_type.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
IONode::read_carrier_type() const
{
  uhal::ValWord<uint32_t> carrier_type = getNode("config.carrier_type").read(); // NOLINT(build/unsigned)
  getClient().dispatch();
  return carrier_type.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
IONode::read_design_type() const
{
  uhal::ValWord<uint32_t> design_type = getNode("config.design_type").read(); // NOLINT(build/unsigned)
  getClient().dispatch();
  return design_type.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
IONode::read_firmware_frequency() const
{
  uhal::ValWord<uint32_t> firmware_frequency = getNode("config.clock_frequency").read(); // NOLINT(build/unsigned)
  getClient().dispatch();
  return firmware_frequency.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
IONode::read_board_uid() const
{

  uint64_t uid = 0;                // NOLINT(build/unsigned)
  std::vector<uint8_t> uid_values = // NOLINT(build/unsigned)
    getNode<I2CMasterNode>(m_uid_i2c_bus).get_slave(m_uid_i2c_device).read_i2cArray(0xfa, 6);

  for (uint8_t i = 0; i < uid_values.size(); ++i) { // NOLINT(build/unsigned)
    uid = (uid << 8) | uid_values.at(i);
  }
  return uid;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
BoardRevision
IONode::get_board_revision() const
{
  auto uid = read_board_uid();
  try {
    return g_board_uid_revision_map.at(uid);
  } catch (const std::out_of_range& e) {
    throw UnknownBoardUID(ERS_HERE, format_reg_value(uid), e);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
IONode::get_hardware_info(bool print_out) const
{
  std::stringstream info;
  const BoardType board_type = convert_value_to_board_type(read_board_type());
  const BoardRevision board_revision = get_board_revision();
  const CarrierType carrier_type = convert_value_to_carrier_type(read_carrier_type());
  const DesignType design_type = convert_value_to_design_type(read_design_type());
  const double firmware_frequency = read_firmware_frequency()/1e6;

  std::vector<std::pair<std::string, std::string>> hardware_info;

  try {
    hardware_info.push_back(std::make_pair("Board type", g_board_type_map.at(board_type)));
  } catch (const std::out_of_range& e) {
    throw MissingBoardTypeMapEntry(ERS_HERE, format_reg_value(board_type), e);
  }

  try {
    hardware_info.push_back(std::make_pair("Board revision", g_board_revision_map.at(board_revision)));
  } catch (const std::out_of_range& e) {
    throw MissingBoardRevisionMapEntry(ERS_HERE, format_reg_value(board_revision), e);
  }

  hardware_info.push_back(std::make_pair("Board UID", format_reg_value(read_board_uid())));

  try {
    hardware_info.push_back(std::make_pair("Carrier type", g_carrier_type_map.at(carrier_type)));
  } catch (const std::out_of_range& e) {
    throw MissingCarrierTypeMapEntry(ERS_HERE, format_reg_value(carrier_type), e);
  }

  try {
    hardware_info.push_back(std::make_pair("Design type", g_design_type_map.at(design_type)));
  } catch (const std::out_of_range& e) {
    throw MissingDesignTypeMapEntry(ERS_HERE, format_reg_value(design_type), e);
  }

  hardware_info.push_back(std::make_pair("Firmware frequency [MHz]", std::to_string(firmware_frequency)));

  info << format_reg_table(hardware_info, "Hardware info", { "", "" });

  if (print_out)
    TLOG() << info.str();
  return info.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
IONode::get_full_clock_config_file_path(const std::string& clock_config_file, int32_t mode) const
{

  if (clock_config_file.size()) {
    // config file provided explicitly, no need for lookup
    return clock_config_file;
    TLOG_DEBUG(0) << "Override pll file: " << clock_config_file;
  } else {

    std::string config_file;
    std::string clock_config_key;

    const BoardRevision board_revision = get_board_revision();
    const CarrierType carrier_type = convert_value_to_carrier_type(read_carrier_type());
    const DesignType design_type = convert_value_to_design_type(read_design_type());
    const uint32_t firmware_frequency = read_firmware_frequency(); // NOLINT(build/unsigned)

    try {
      clock_config_key = g_board_revision_map.at(board_revision) + "_";
    } catch (const std::out_of_range& e) {
      throw MissingBoardRevisionMapEntry(ERS_HERE, format_reg_value(board_revision), e);
    }

    try {
      clock_config_key = clock_config_key + g_carrier_type_map.at(carrier_type) + "_";
    } catch (const std::out_of_range& e) {
      throw MissingCarrierTypeMapEntry(ERS_HERE, format_reg_value(carrier_type), e);
    }

    try {
      clock_config_key = clock_config_key + g_design_type_map.at(design_type);
    } catch (const std::out_of_range& e) {
      throw MissingDesignTypeMapEntry(ERS_HERE, format_reg_value(design_type), e);
    }

    // modifier in case a different clock file is needed based on firmware configuration
    if (mode >= 0)
      clock_config_key = clock_config_key + "_mode" + std::to_string(mode);

    // 50 MHz firmware clock frequency modifier, otherwise assume 62.5 MHz
    if (firmware_frequency == 50e6) {
      clock_config_key = clock_config_key + "_50_mhz";
    } else if (firmware_frequency != 62.5e6) {
      throw UnknownFirmwareClockFrequency(ERS_HERE, firmware_frequency);
    }

    TLOG_DEBUG(0) << "Using pll config key: " << clock_config_key;

    try {
      config_file = g_clock_config_map.at(clock_config_key);
    } catch (const std::out_of_range& e) {
      throw ClockConfigNotFound(ERS_HERE, clock_config_key, e);
    }

    TLOG_DEBUG(0) << "PLL config file: " << config_file << " from key: " << clock_config_key;

    const char* env_var_char = std::getenv("TIMING_SHARE");

    if (env_var_char == nullptr) {
      throw EnvironmentVariableNotSet(ERS_HERE, "TIMING_SHARE");
    }

    std::string env_var(env_var_char);

    std::string full_pll_config_file_path = env_var + "/config/etc/clock/" + config_file;

    TLOG_DEBUG(0) << "Full PLL config file path: " << full_pll_config_file_path;

    return full_pll_config_file_path;
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::unique_ptr<const SI534xSlave>
IONode::get_pll() const
{
  return get_i2c_device<SI534xSlave>(m_pll_i2c_bus, m_pll_i2c_device);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
IONode::configure_pll(const std::string& clock_config_file) const
{
  auto pll = get_pll();

  uint32_t si_pll_version = pll->read_device_version(); // NOLINT(build/unsigned)
  TLOG_DEBUG(0) << "Configuring PLL        : SI" << format_reg_value(si_pll_version);

  pll->configure(clock_config_file);

  TLOG_DEBUG(0) << "PLL configuration id   : " << pll->read_config_id();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<double>
IONode::read_clock_frequencies() const
{
  return getNode<FrequencyCounterNode>("freq").measure_frequencies(m_clock_names.size());
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
IONode::get_clock_frequencies_table(bool print_out) const
{
  std::stringstream table;
  std::vector<double> frequencies = read_clock_frequencies();
  for (uint8_t i = 0; i < frequencies.size(); ++i) { // NOLINT(build/unsigned)
    table << m_clock_names.at(i) << " freq: " << std::setprecision(12) << frequencies.at(i) << std::endl;
  }
  // TODO add freq validation Stoyan Trilov stoyan.trilov@cern.ch
  if (print_out)
    TLOG() << table.str();
  return table.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
IONode::get_pll_status(bool print_out) const
{

  std::stringstream status;

  auto pll = get_pll();
  status << "PLL configuration id   : " << pll->read_config_id() << std::endl;

  std::map<std::string, uint32_t> pll_version; // NOLINT(build/unsigned)
  pll_version["Part number"] = pll->read_device_version();
  pll_version["Device grade"] = pll->read_clock_register(0x4);
  pll_version["Device revision"] = pll->read_clock_register(0x5);

  status << format_reg_table(pll_version, "PLL information") << std::endl;

  std::map<std::string, uint32_t> pll_registers; // NOLINT(build/unsigned)

  uint8_t pll_reg_c = pll->read_clock_register(0xc);   // NOLINT(build/unsigned)
  uint8_t pll_reg_d = pll->read_clock_register(0xd);   // NOLINT(build/unsigned)
  uint8_t pll_reg_e = pll->read_clock_register(0xe);   // NOLINT(build/unsigned)
  uint8_t pll_reg_f = pll->read_clock_register(0xf);   // NOLINT(build/unsigned)
  uint8_t pll_reg_11 = pll->read_clock_register(0x11); // NOLINT(build/unsigned)
  uint8_t pll_reg_12 = pll->read_clock_register(0x12); // NOLINT(build/unsigned)

  pll_registers["CAL_PLL"] = dec_rng(pll_reg_f, 5);
  pll_registers["HOLD"] = dec_rng(pll_reg_e, 5);
  pll_registers["LOL"] = dec_rng(pll_reg_e, 1);
  pll_registers["LOS"] = dec_rng(pll_reg_d, 0, 4);
  pll_registers["LOSXAXB"] = dec_rng(pll_reg_c, 1);
  pll_registers["LOSXAXB_FLG"] = dec_rng(pll_reg_11, 1);

  pll_registers["OOF"] = dec_rng(pll_reg_d, 4, 4);
  pll_registers["OOF (sticky)"] = dec_rng(pll_reg_12, 4, 4);

  pll_registers["SMBUS_TIMEOUT"] = dec_rng(pll_reg_c, 5);
  pll_registers["SMBUS_TIMEOUT_FLG"] = dec_rng(pll_reg_11, 5);

  pll_registers["SYSINCAL"] = dec_rng(pll_reg_c, 0);
  pll_registers["SYSINCAL_FLG"] = dec_rng(pll_reg_11, 0);

  pll_registers["XAXB_ERR"] = dec_rng(pll_reg_c, 3);
  pll_registers["XAXB_ERR_FLG"] = dec_rng(pll_reg_11, 3);

  status << format_reg_table(pll_registers, "PLL state");

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
IONode::write_soft_reset_register() const
{
  getNode("csr.ctrl.soft_rst").write(0x1);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
IONode::soft_reset() const
{
  write_soft_reset_register();
  TLOG_DEBUG(0) << "Soft reset done";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
IONode::get_sfp_status(uint32_t sfp_id, bool print_out) const // NOLINT(build/unsigned)
{
  std::stringstream status;
  std::string sfp_i2c_bus;
  try {
    sfp_i2c_bus = m_sfp_i2c_buses.at(sfp_id);
  } catch (const std::out_of_range& e) {
    throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id), e);
  }
  auto sfp = get_i2c_device<I2CSFPSlave>(sfp_i2c_bus, "SFP_EEProm");
  status << sfp->get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
IONode::switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const // NOLINT(build/unsigned)
{
  std::string sfp_i2c_bus;
  try {
    sfp_i2c_bus = m_sfp_i2c_buses.at(sfp_id);
  } catch (const std::out_of_range& e) {
    throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id), e);
  }
  auto sfp = get_i2c_device<I2CSFPSlave>(sfp_i2c_bus, "SFP_EEProm");
  sfp->switch_soft_tx_control_bit(turn_on);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq