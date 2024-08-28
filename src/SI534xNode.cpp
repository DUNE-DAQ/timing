/**
 * @file SI534xNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/SI534xNode.hpp"

// PDT headers
#include "ers/ers.hpp"
#include "logging/Logging.hpp"

#include "timing/toolbox.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/tuple/tuple.hpp>

#include <chrono>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace dunedaq {
namespace timing {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(SI534xNode)

//-----------------------------------------------------------------------------
SI534xSlave::SI534xSlave(const I2CMasterNode* i2c_master, uint8_t address) // NOLINT(build/unsigned)
  : SIChipSlave(i2c_master, address)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SI534xSlave::~SI534xSlave() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
SI534xSlave::read_config_id() const
{
  std::string id;

  for (size_t i(0); i < 8; ++i) {
    char id_char = static_cast<char>(read_clock_register(0x26b + i));
    // in case null paddding was used for spaces
    if (id_char == '\0')
    {
      id += ' ';
    }
    else
    {
      id += id_char;
    }
  }
  return id;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
SI534xSlave::seek_header(std::ifstream& file) const
{

  // std::string config_line;
  std::string design_id;

  std::string config_line;
  uint32_t line_number; // NOLINT(build/unsigned)
  for (line_number = 1; std::getline(file, config_line); ++line_number) {

    // Gracefully deal with those damn dos-encoded files
    if (config_line.back() == '\r')
      config_line.pop_back();

    // Section end found. Break here
    if (boost::starts_with(config_line, "# Design ID:")) {
      design_id = config_line.substr(13);
    }

    // Skip comments
    if (config_line[0] == '#')
      continue;

    // Stop if the line is empty
    if (config_line.length() == 0)
      continue;

    // OK, header found, stop here
    if (config_line == "Address,Data")
      break;

    if (file.eof()) {
      throw SI534xConfigError(ERS_HERE, "Incomplete file: End of file detected while seeking the header.");
    }
  }

  TLOG_DEBUG(8) << "Found desing ID " << design_id;

  return design_id;
}

//-----------------------------------------------------------------------------
// Seek Header
// Seek conf start
// read data
// Stop on conf end

std::vector<SI534xSlave::RegisterSetting_t>
SI534xSlave::read_config_section(std::ifstream& file, std::string tag) const
{

  // Line buffer
  // std::string config_line;

  bool section_found(false);

  std::vector<RegisterSetting_t> config;
  std::string config_line;
  uint32_t line_number; // NOLINT(build/unsigned)
  for (line_number = 1; std::getline(file, config_line); ++line_number) {

    // Gracefully deal with those damn dos-encoded files
    if (config_line.back() == '\r')
      config_line.pop_back();

    // Is it a comment
    if (config_line[0] == '#') {

      if (tag.empty())
        continue;

      if (boost::starts_with(config_line, "# Start configuration " + tag)) {
        section_found = true;
      }

      // Section end found. Break here
      if (boost::starts_with(config_line, "# End configuration " + tag)) {
        break;
      }

      continue;
    }

    // Oops
    if (file.eof()) {
      if (tag.empty())
        return config;
      else
        throw SI534xConfigError(ERS_HERE,
                                "Incomplete file: End of file detected before the end of " + tag + " section.");
    }

    // Stop if the line is empty
    if (config_line.length() == 0)
      continue;

    // If no sec
    if (!section_found && !tag.empty()) {
      throw SI534xMissingConfigSectionError(ERS_HERE, tag);
    }

    uint32_t address, data; // NOLINT(build/unsigned)
    char dummy;

    std::istringstream line_stream(config_line);
    line_stream >> std::hex >> address >> dummy >> std::hex >> data;

    std::stringstream debug_stream;
    debug_stream << std::showbase << std::hex << "Address: " << address << dummy << " Data: " << data;
    TLOG_DEBUG(8) << debug_stream.str();

    config.push_back(RegisterSetting_t(address, data));
  }

  return config;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SI534xSlave::configure(const std::string& filename) const
{

  throw_if_not_file(filename);

  std::ifstream config_file(filename);
  std::string config_line;
  std::string conf_design_id;

  // Seek the header line first
  conf_design_id = seek_header(config_file);
  std::ifstream::pos_type header_end = config_file.tellg();

  // auto preamble = this->read_config_section(config_file, "preamble");
  // TIMING_LOG(kDebug) << "Preamble size = " << preamble.size();
  // auto registers = this->read_config_section(config_file, "registers");
  // TIMING_LOG(kDebug) << "Registers size = " << registers.size();
  // auto postamble = this->read_config_section(config_file, "postamble");
  // TIMING_LOG(kDebug) << "PostAmble size = " << postamble.size();

  std::vector<SI534xSlave::RegisterSetting_t> preamble, registers, postamble;

  try {
    preamble = this->read_config_section(config_file, "preamble");
    registers = this->read_config_section(config_file, "registers");
    postamble = this->read_config_section(config_file, "postamble");
  } catch (SI534xMissingConfigSectionError&) {
    config_file.seekg(header_end);
    preamble.clear();
    registers = this->read_config_section(config_file, "");
    postamble.clear();
  }

  TLOG_DEBUG(8) << "Preamble size = " << preamble.size();
  TLOG_DEBUG(8) << "Registers size = " << registers.size();
  TLOG_DEBUG(8) << "PostAmble size = " << postamble.size();

  config_file.close();

  try {
    this->write_clock_register(0x1E, 0x2);
  } catch (timing::I2CException& excp) {
    // Do nothing.
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  this->upload_config(preamble);
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  this->upload_config(registers);
  this->upload_config(postamble);

  std::string chip_design_id = this->read_config_id();

  if (conf_design_id != chip_design_id) {
    std::ostringstream message;
    message << "Post-configuration check failed: Loaded design ID " << chip_design_id
         << " does not match the configurationd design id " << conf_design_id << std::endl;
    ers::error(SI534xConfigError(ERS_HERE, message.str()));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SI534xSlave::upload_config(const std::vector<SI534xSlave::RegisterSetting_t>& config) const
{

  size_t k(0), notify_percent(10);
  size_t notify_every = (notify_percent < config.size() ? config.size() / notify_percent : 1);

  for (const auto& setting : config) {
    std::stringstream debug_stream;
    debug_stream << std::showbase << std::hex << "Writing to " << (uint32_t)setting.get<0>() // NOLINT(build/unsigned)
                 << " data " << (uint32_t)setting.get<1>();                                  // NOLINT(build/unsigned)
    TLOG_DEBUG(9) << debug_stream.str();

    uint32_t max_attempts(2), attempt(0); // NOLINT(build/unsigned)
    while (attempt < max_attempts) {
      TLOG_DEBUG(9) << "Attempt " << attempt;
      if (attempt > 0) {
        ers::warning(SI534xRegWriteRetry(ERS_HERE,
                                         format_reg_value(attempt, 10),
                                         format_reg_value((uint32_t)setting.get<0>()))); // NOLINT(build/unsigned)
      }
      try {
        this->write_clock_register(setting.get<0>(), setting.get<1>());
      } catch (const std::exception& e) {
        ers::error(SI534xRegWriteFailed(ERS_HERE,
                                        format_reg_value((uint32_t)setting.get<0>()), // NOLINT(build/unsigned)
                                        format_reg_value((uint32_t)setting.get<1>()), // NOLINT(build/unsigned)
                                        e));
        ++attempt;
        continue;
      }
      break;
    }

    ++k;
    if ((k % notify_every) == 0) {
      TLOG_DEBUG(9) << (k / notify_every) * notify_percent << "%";
    }
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::map<uint16_t, uint8_t> // NOLINT(build/unsigned)
SI534xSlave::registers() const
{
  //  boost::format fmthex("%d");
  std::map<uint16_t, uint8_t> values; // NOLINT(build/unsigned)

  for (uint8_t reg_addr = 0xc; reg_addr <= 0x12; reg_addr++) { // NOLINT(build/unsigned)
    if (reg_addr > 0xf && reg_addr < 0x11) {
      continue;
    }

    //     if( reg_addr > 25 && reg_addr < 31 ) {
    //         continue;
    //     }

    //     if( reg_addr > 36 && reg_addr < 40 ) {
    //         continue;
    //     }

    //     if( reg_addr > 48 && reg_addr < 55 ) {
    //         continue;
    //     }

    //     if( reg_addr > 55 && reg_addr < 128 ) {
    //         continue;
    //     }

    //     if( reg_addr == 133 ) {
    //         continue;
    //     }

    //     if( reg_addr == 137 ) {
    //         continue;
    //     }

    //     if( reg_addr > 139 && reg_addr < 142 ) {
    //         continue;
    //     }

    values[reg_addr] = read_clock_register(reg_addr);
  }

  return values;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
 void
 SI534xSlave::get_info(timinghardwareinfo::TimingPLLMonitorData& mon_data) const
 {
  mon_data.config_id = this->read_config_id();

  //lPLLVersion["Part number"] = pll->read_device_version();
  //lPLLVersion["Device grade"] = pll->read_clock_register(0x4);
  //lPLLVersion["Device revision"] = pll->read_clock_register(0x5);

  uint8_t pll_reg_c = this->read_clock_register(0xc);   // NOLINT(build/unsigned)
  uint8_t pll_reg_d = this->read_clock_register(0xd);   // NOLINT(build/unsigned)
  uint8_t pll_reg_e = this->read_clock_register(0xe);   // NOLINT(build/unsigned)
  uint8_t pll_reg_f = this->read_clock_register(0xf);   // NOLINT(build/unsigned)
  uint8_t pll_reg_11 = this->read_clock_register(0x11); // NOLINT(build/unsigned)
  uint8_t pll_reg_12 = this->read_clock_register(0x12); // NOLINT(build/unsigned)

  mon_data.cal_pll = dec_rng(pll_reg_f, 5);
  mon_data.hold = dec_rng(pll_reg_e, 5);
  mon_data.lol = dec_rng(pll_reg_e, 1);
  mon_data.los = dec_rng(pll_reg_d, 0, 4);
  mon_data.los_xaxb = dec_rng(pll_reg_c, 1);
  mon_data.los_xaxb_flg = dec_rng(pll_reg_11, 1);

  mon_data.oof = dec_rng(pll_reg_d, 4, 4);
  mon_data.oof_sticky = dec_rng(pll_reg_12, 4, 4);

  mon_data.smbus_timeout = dec_rng(pll_reg_c, 5);
  mon_data.smbus_timeout_flg = dec_rng(pll_reg_11, 5);

  mon_data.sys_in_cal = dec_rng(pll_reg_c, 0);
  mon_data.sys_in_cal_flg = dec_rng(pll_reg_11, 0);

  mon_data.xaxb_err = dec_rng(pll_reg_c, 3);
  mon_data.xaxb_err_flg = dec_rng(pll_reg_11, 3);
 }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SI534xNode::SI534xNode(const uhal::Node& node)
  : I2CMasterNode(node)
  , SI534xSlave(this, this->get_slave_address("i2caddr"))
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SI534xNode::SI534xNode(const SI534xNode& node)
  : I2CMasterNode(node)
  , SI534xSlave(this, this->get_slave_address("i2caddr"))
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SI534xNode::~SI534xNode() {}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
