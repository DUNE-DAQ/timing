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
  uhal::ValWord<uint32_t> lBoardType = getNode("config.board_type").read(); // NOLINT(build/unsigned)
  getClient().dispatch();
  return lBoardType.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
IONode::read_carrier_type() const
{
  uhal::ValWord<uint32_t> lCarrierType = getNode("config.carrier_type").read(); // NOLINT(build/unsigned)
  getClient().dispatch();
  return lCarrierType.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
IONode::read_design_type() const
{
  uhal::ValWord<uint32_t> lDesignType = getNode("config.design_type").read(); // NOLINT(build/unsigned)
  getClient().dispatch();
  return lDesignType.value();
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

  uint64_t lUID = 0;                // NOLINT(build/unsigned)
  std::vector<uint8_t> lUIDValues = // NOLINT(build/unsigned)
    getNode<I2CMasterNode>(m_uid_i2c_bus).get_slave(m_uid_i2c_device).read_i2cArray(0xfa, 6);

  for (uint8_t i = 0; i < lUIDValues.size(); ++i) { // NOLINT(build/unsigned)
    lUID = (lUID << 8) | lUIDValues.at(i);
  }
  return lUID;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
BoardRevision
IONode::get_board_revision() const
{

  auto lUID = read_board_uid();
  try {
    return g_board_uid_revision_map.at(lUID);
  } catch (const std::out_of_range& e) {
    throw UnknownBoardUID(ERS_HERE, format_reg_value(lUID), e);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
IONode::get_hardware_info(bool print_out) const
{
  std::stringstream lInfo;
  const BoardType lBoardType = convert_value_to_board_type(read_board_type());
  const BoardRevision lBoardRevision = get_board_revision();
  const CarrierType lCarrierType = convert_value_to_carrier_type(read_carrier_type());
  const DesignType lDesignType = convert_value_to_design_type(read_design_type());
  const double firmware_frequency = read_firmware_frequency()/1e6;

  std::vector<std::pair<std::string, std::string>> lHardwareInfo;

  // TODO check map at exception
  try {
    lHardwareInfo.push_back(std::make_pair("Board type", g_board_type_map.at(lBoardType)));
  } catch (const std::out_of_range& e) {
    throw MissingBoardTypeMapEntry(ERS_HERE, format_reg_value(lBoardType), e);
  }

  try {
    lHardwareInfo.push_back(std::make_pair("Board revision", g_board_revision_map.at(lBoardRevision)));
  } catch (const std::out_of_range& e) {
    throw MissingBoardRevisionMapEntry(ERS_HERE, format_reg_value(lBoardRevision), e);
  }

  lHardwareInfo.push_back(std::make_pair("Board UID", format_reg_value(read_board_uid())));

  try {
    lHardwareInfo.push_back(std::make_pair("Carrier type", g_carrier_type_map.at(lCarrierType)));
  } catch (const std::out_of_range& e) {
    throw MissingCarrierTypeMapEntry(ERS_HERE, format_reg_value(lCarrierType), e);
  }

  try {
    lHardwareInfo.push_back(std::make_pair("Design type", g_design_type_map.at(lDesignType)));
  } catch (const std::out_of_range& e) {
    throw MissingDesignTypeMapEntry(ERS_HERE, format_reg_value(lDesignType), e);
  }

  lHardwareInfo.push_back(std::make_pair("Firmware frequency [MHz]", std::to_string(firmware_frequency)));

  lInfo << format_reg_table(lHardwareInfo, "Hardware info", { "", "" });

  if (print_out)
    TLOG() << lInfo.str();
  return lInfo.str();
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

    std::string lConfigFile;
    std::string lClockConfigKey;

    const BoardRevision lBoardRevision = get_board_revision();
    const CarrierType lCarrierType = convert_value_to_carrier_type(read_carrier_type());
    const DesignType lDesignType = convert_value_to_design_type(read_design_type());
    const uint32_t firmware_frequency = read_firmware_frequency();    

    try {
      lClockConfigKey = g_board_revision_map.at(lBoardRevision) + "_";
    } catch (const std::out_of_range& e) {
      throw MissingBoardRevisionMapEntry(ERS_HERE, format_reg_value(lBoardRevision), e);
    }

    try {
      lClockConfigKey = lClockConfigKey + g_carrier_type_map.at(lCarrierType) + "_";
    } catch (const std::out_of_range& e) {
      throw MissingCarrierTypeMapEntry(ERS_HERE, format_reg_value(lCarrierType), e);
    }

    try {
      lClockConfigKey = lClockConfigKey + g_design_type_map.at(lDesignType);
    } catch (const std::out_of_range& e) {
      throw MissingDesignTypeMapEntry(ERS_HERE, format_reg_value(lDesignType), e);
    }

    // modifier in case a different clock file is needed based on firmware configuration
    if (mode >= 0)
      lClockConfigKey = lClockConfigKey + "_mode" + std::to_string(mode);

    // 50 MHz firmware clock frequency modifier, otherwise assume 62.5 MHz
    if (firmware_frequency == 50e6) {
      lClockConfigKey = lClockConfigKey + "_50_mhz";
    } else if (firmware_frequency != 62.5e6) {
      throw UnknownFirmwareClockFrequency(ERS_HERE, firmware_frequency);
    }

    TLOG_DEBUG(0) << "Using pll config key: " << lClockConfigKey;

    try {
      lConfigFile = g_clock_config_map.at(lClockConfigKey);
    } catch (const std::out_of_range& e) {
      throw ClockConfigNotFound(ERS_HERE, lClockConfigKey, e);
    }

    TLOG_DEBUG(0) << "PLL config file: " << lConfigFile << " from key: " << lClockConfigKey;

    const char* env_var_char = std::getenv("TIMING_SHARE");

    if (env_var_char == nullptr) {
      throw EnvironmentVariableNotSet(ERS_HERE, "TIMING_SHARE");
    }

    std::string env_var(env_var_char);

    std::string full_pll_config_file_path = env_var + "/config/etc/clock/" + lConfigFile;

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

  uint32_t lSIVersion = pll->read_device_version(); // NOLINT(build/unsigned)
  TLOG_DEBUG(0) << "Configuring PLL        : SI" << format_reg_value(lSIVersion);

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
  std::stringstream lTable;
  std::vector<double> lFrequencies = read_clock_frequencies();
  for (uint8_t i = 0; i < lFrequencies.size(); ++i) { // NOLINT(build/unsigned)
    lTable << m_clock_names.at(i) << " freq: " << std::setprecision(12) << lFrequencies.at(i) << std::endl;
  }
  // TODO add freq validation
  if (print_out)
    TLOG() << lTable.str();
  return lTable.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
IONode::get_pll_status(bool print_out) const
{

  std::stringstream lStatus;

  auto pll = get_pll();
  lStatus << "PLL configuration id   : " << pll->read_config_id() << std::endl;

  std::map<std::string, uint32_t> lPLLVersion; // NOLINT(build/unsigned)
  lPLLVersion["Part number"] = pll->read_device_version();
  lPLLVersion["Device grade"] = pll->read_clock_register(0x4);
  lPLLVersion["Device revision"] = pll->read_clock_register(0x5);

  lStatus << format_reg_table(lPLLVersion, "PLL information") << std::endl;

  std::map<std::string, uint32_t> lPLLRegisters; // NOLINT(build/unsigned)

  uint8_t lPLLReg_c = pll->read_clock_register(0xc);   // NOLINT(build/unsigned)
  uint8_t lPLLReg_d = pll->read_clock_register(0xd);   // NOLINT(build/unsigned)
  uint8_t lPLLReg_e = pll->read_clock_register(0xe);   // NOLINT(build/unsigned)
  uint8_t lPLLReg_f = pll->read_clock_register(0xf);   // NOLINT(build/unsigned)
  uint8_t lPLLReg_11 = pll->read_clock_register(0x11); // NOLINT(build/unsigned)
  uint8_t lPLLReg_12 = pll->read_clock_register(0x12); // NOLINT(build/unsigned)

  lPLLRegisters["CAL_PLL"] = dec_rng(lPLLReg_f, 5);
  lPLLRegisters["HOLD"] = dec_rng(lPLLReg_e, 5);
  lPLLRegisters["LOL"] = dec_rng(lPLLReg_e, 1);
  lPLLRegisters["LOS"] = dec_rng(lPLLReg_d, 0, 4);
  lPLLRegisters["LOSXAXB"] = dec_rng(lPLLReg_c, 1);
  lPLLRegisters["LOSXAXB_FLG"] = dec_rng(lPLLReg_11, 1);

  lPLLRegisters["OOF"] = dec_rng(lPLLReg_d, 4, 4);
  lPLLRegisters["OOF (sticky)"] = dec_rng(lPLLReg_12, 4, 4);

  lPLLRegisters["SMBUS_TIMEOUT"] = dec_rng(lPLLReg_c, 5);
  lPLLRegisters["SMBUS_TIMEOUT_FLG"] = dec_rng(lPLLReg_11, 5);

  lPLLRegisters["SYSINCAL"] = dec_rng(lPLLReg_c, 0);
  lPLLRegisters["SYSINCAL_FLG"] = dec_rng(lPLLReg_11, 0);

  lPLLRegisters["XAXB_ERR"] = dec_rng(lPLLReg_c, 3);
  lPLLRegisters["XAXB_ERR_FLG"] = dec_rng(lPLLReg_11, 3);

  lStatus << format_reg_table(lPLLRegisters, "PLL state");

  if (print_out)
    TLOG() << lStatus.str();
  return lStatus.str();
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
  std::stringstream lStatus;
  std::string lSFPI2CBus;
  try {
    lSFPI2CBus = m_sfp_i2c_buses.at(sfp_id);
  } catch (const std::out_of_range& e) {
    throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id), e);
  }
  auto sfp = get_i2c_device<I2CSFPSlave>(lSFPI2CBus, "SFP_EEProm");
  lStatus << sfp->get_status();
  if (print_out)
    TLOG() << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
IONode::switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const // NOLINT(build/unsigned)
{
  std::string lSFPI2CBus;
  try {
    lSFPI2CBus = m_sfp_i2c_buses.at(sfp_id);
  } catch (const std::out_of_range& e) {
    throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id), e);
  }
  auto sfp = get_i2c_device<I2CSFPSlave>(lSFPI2CBus, "SFP_EEProm");
  sfp->switch_soft_tx_control_bit(turn_on);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq