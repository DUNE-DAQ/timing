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
    id += static_cast<char>(read_clock_register(0x26b + i));
  }
  return id;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
SI534xSlave::seek_header(std::ifstream& file) const
{

  // std::string lLine;
  std::string lDesignID;

  std::string lLine;
  uint32_t lLineNum; // NOLINT(build/unsigned)
  for (lLineNum = 1; std::getline(file, lLine); ++lLineNum) {

    // Gracefully deal with those damn dos-encoded files
    if (lLine.back() == '\r')
      lLine.pop_back();

    // Section end found. Break here
    if (boost::starts_with(lLine, "# Design ID:")) {
      lDesignID = lLine.substr(13);
    }

    // Skip comments
    if (lLine[0] == '#')
      continue;

    // Stop if the line is empty
    if (lLine.length() == 0)
      continue;

    // OK, header found, stop here
    if (lLine == "Address,Data")
      break;

    if (file.eof()) {
      throw SI534xConfigError(ERS_HERE, "Incomplete file: End of file detected while seeking the header.");
    }
  }

  TLOG_DEBUG(2) << "Found desing ID " << lDesignID;

  return lDesignID;
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
  // std::string lLine;

  bool lSectionFound(false);

  std::vector<RegisterSetting_t> lConfig;
  std::string lLine;
  uint32_t lLineNum; // NOLINT(build/unsigned)
  for (lLineNum = 1; std::getline(file, lLine); ++lLineNum) {

    // Gracefully deal with those damn dos-encoded files
    if (lLine.back() == '\r')
      lLine.pop_back();

    // Is it a comment
    if (lLine[0] == '#') {

      if (tag.empty())
        continue;

      if (boost::starts_with(lLine, "# Start configuration " + tag)) {
        lSectionFound = true;
      }

      // Section end found. Break here
      if (boost::starts_with(lLine, "# End configuration " + tag)) {
        break;
      }

      continue;
    }

    // Oops
    if (file.eof()) {
      if (tag.empty())
        return lConfig;
      else
        throw SI534xConfigError(ERS_HERE,
                                "Incomplete file: End of file detected before the end of " + tag + " section.");
    }

    // Stop if the line is empty
    if (lLine.length() == 0)
      continue;

    // If no sec
    if (!lSectionFound && !tag.empty()) {
      throw SI534xMissingConfigSectionError(ERS_HERE, tag);
    }

    uint32_t lAddress, lData; // NOLINT(build/unsigned)
    char lDummy;

    std::istringstream lLineStream(lLine);
    lLineStream >> std::hex >> lAddress >> lDummy >> std::hex >> lData;

    std::stringstream debug_stream;
    debug_stream << std::showbase << std::hex << "Address: " << lAddress << lDummy << " Data: " << lData;
    TLOG_DEBUG(3) << debug_stream.str();

    lConfig.push_back(RegisterSetting_t(lAddress, lData));
  }

  return lConfig;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SI534xSlave::configure(const std::string& aPath) const
{

  throw_if_not_file(aPath);

  std::ifstream lFile(aPath);
  std::string lLine;
  std::string lConfDesignID;

  // Seek the header line first
  lConfDesignID = seek_header(lFile);
  std::ifstream::pos_type lHdrEnd = lFile.tellg();

  // auto lPreamble = this->read_config_section(lFile, "preamble");
  // TIMING_LOG(kDebug) << "Preamble size = " << lPreamble.size();
  // auto lRegisters = this->read_config_section(lFile, "registers");
  // TIMING_LOG(kDebug) << "Registers size = " << lRegisters.size();
  // auto lPostAmble = this->read_config_section(lFile, "postamble");
  // TIMING_LOG(kDebug) << "PostAmble size = " << lPostAmble.size();

  std::vector<SI534xSlave::RegisterSetting_t> lPreamble, lRegisters, lPostAmble;

  try {
    lPreamble = this->read_config_section(lFile, "preamble");
    lRegisters = this->read_config_section(lFile, "registers");
    lPostAmble = this->read_config_section(lFile, "postamble");
  } catch (SI534xMissingConfigSectionError&) {
    lFile.seekg(lHdrEnd);
    lPreamble.clear();
    lRegisters = this->read_config_section(lFile, "");
    lPostAmble.clear();
  }

  TLOG_DEBUG(3) << "Preamble size = " << lPreamble.size();
  TLOG_DEBUG(3) << "Registers size = " << lRegisters.size();
  TLOG_DEBUG(3) << "PostAmble size = " << lPostAmble.size();

  lFile.close();

  try {
    this->write_clock_register(0x1E, 0x2);
  } catch (timing::I2CException& lExc) {
    // Do nothing.
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  this->upload_config(lPreamble);
  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  this->upload_config(lRegisters);
  this->upload_config(lPostAmble);

  std::string lChipDesignID = this->read_config_id();

  if (lConfDesignID != lChipDesignID) {
    std::ostringstream lMsg;
    lMsg << "Post-configuration check failed: Loaded design ID " << lChipDesignID
         << " does not match the configurationd design id " << lConfDesignID << std::endl;
    throw SI534xConfigError(ERS_HERE, lMsg.str());
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SI534xSlave::upload_config(const std::vector<SI534xSlave::RegisterSetting_t>& config) const
{

  size_t k(0), lNotifyPercent(10);
  size_t lNotifyEvery = (lNotifyPercent < config.size() ? config.size() / lNotifyPercent : 1);

  for (const auto& lSetting : config) {
    std::stringstream debug_stream;
    debug_stream << std::showbase << std::hex << "Writing to " << (uint32_t)lSetting.get<0>() // NOLINT(build/unsigned)
                 << " data " << (uint32_t)lSetting.get<1>();                                  // NOLINT(build/unsigned)
    TLOG_DEBUG(3) << debug_stream.str();

    uint32_t lMaxAttempts(2), lAttempt(0); // NOLINT(build/unsigned)
    while (lAttempt < lMaxAttempts) {
      TLOG_DEBUG(3) << "Attempt " << lAttempt;
      if (lAttempt > 0) {
        ers::warning(SI534xRegWriteRetry(ERS_HERE,
                                         format_reg_value(lAttempt, 10),
                                         format_reg_value((uint32_t)lSetting.get<0>()))); // NOLINT(build/unsigned)
      }
      try {
        this->write_clock_register(lSetting.get<0>(), lSetting.get<1>());
      } catch (const std::exception& e) {
        ers::error(SI534xRegWriteFailed(ERS_HERE,
                                        format_reg_value((uint32_t)lSetting.get<0>()), // NOLINT(build/unsigned)
                                        format_reg_value((uint32_t)lSetting.get<1>()), // NOLINT(build/unsigned)
                                        e));
        ++lAttempt;
        continue;
      }
      break;
    }

    ++k;
    if ((k % lNotifyEvery) == 0) {
      TLOG_DEBUG(3) << (k / lNotifyEvery) * lNotifyPercent << "%";
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

  for (uint8_t regAddr = 0xc; regAddr <= 0x12; regAddr++) { // NOLINT(build/unsigned)
    if (regAddr > 0xf && regAddr < 0x11) {
      continue;
    }

    //     if( regAddr > 25 && regAddr < 31 ) {
    //         continue;
    //     }

    //     if( regAddr > 36 && regAddr < 40 ) {
    //         continue;
    //     }

    //     if( regAddr > 48 && regAddr < 55 ) {
    //         continue;
    //     }

    //     if( regAddr > 55 && regAddr < 128 ) {
    //         continue;
    //     }

    //     if( regAddr == 133 ) {
    //         continue;
    //     }

    //     if( regAddr == 137 ) {
    //         continue;
    //     }

    //     if( regAddr > 139 && regAddr < 142 ) {
    //         continue;
    //     }

    values[regAddr] = read_clock_register(regAddr);
  }

  return values;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SI534xSlave::get_info(timinghardwareinfo::TimingPLLMonitorData& mon_data) const
{

  mon_data.config_id = this->read_config_id();

  // lPLLVersion["Part number"] = pll->read_device_version();
  // lPLLVersion["Device grade"] = pll->read_clock_register(0x4);
  // lPLLVersion["Device revision"] = pll->read_clock_register(0x5);

  uint8_t lPLLReg_c = this->read_clock_register(0xc);   // NOLINT(build/unsigned)
  uint8_t lPLLReg_d = this->read_clock_register(0xd);   // NOLINT(build/unsigned)
  uint8_t lPLLReg_e = this->read_clock_register(0xe);   // NOLINT(build/unsigned)
  uint8_t lPLLReg_f = this->read_clock_register(0xf);   // NOLINT(build/unsigned)
  uint8_t lPLLReg_11 = this->read_clock_register(0x11); // NOLINT(build/unsigned)
  uint8_t lPLLReg_12 = this->read_clock_register(0x12); // NOLINT(build/unsigned)

  mon_data.cal_pll = dec_rng(lPLLReg_f, 5);
  mon_data.hold = dec_rng(lPLLReg_e, 5);
  mon_data.lol = dec_rng(lPLLReg_e, 1);
  mon_data.los = dec_rng(lPLLReg_d, 0, 4);
  mon_data.los_xaxb = dec_rng(lPLLReg_c, 1);
  mon_data.los_xaxb_flg = dec_rng(lPLLReg_11, 1);

  mon_data.oof = dec_rng(lPLLReg_d, 4, 4);
  mon_data.oof_sticky = dec_rng(lPLLReg_12, 4, 4);

  mon_data.smbus_timeout = dec_rng(lPLLReg_c, 5);
  mon_data.smbus_timeout_flg = dec_rng(lPLLReg_11, 5);

  mon_data.sys_in_cal = dec_rng(lPLLReg_c, 0);
  mon_data.sys_in_cal_flg = dec_rng(lPLLReg_11, 0);

  mon_data.xaxb_err = dec_rng(lPLLReg_c, 3);
  mon_data.xaxb_err_flg = dec_rng(lPLLReg_11, 3);
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