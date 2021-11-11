/**
 * @file TLUIONode.hpp
 *
 * TLUIONode is a class providing an interface
 * to the TLU IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_TLUIONODE_HPP_
#define TIMING_INCLUDE_TIMING_TLUIONODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/IONode.hpp"
#include "timing/timinghardwareinfo/InfoStructs.hpp"
#include "timing/timinghardwareinfo/InfoNljs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for the TLU board.
 */
class TLUIONode : public IONode
{
  UHAL_DERIVEDNODE(TLUIONode)

public:
  explicit TLUIONode(const uhal::Node& node);
  virtual ~TLUIONode();

  /**
   * @brief      Get the UID address parameter name.
   *
   * @return     { description_of_the_return_value }
   */
  std::string get_uid_address_parameter_name() const override;
  
  /**
   * @brief     Print the status of the timing node.
   */
  std::string get_status(bool print_out = false) const;

  /**
   * @brief      Reset timing node.
   */
  void reset(const std::string& clock_config_file = "") const override;

  /**
   * @brief      Configure on-board DAC
   */
  void configure_dac(uint32_t dac_id, uint32_t dac_value, bool internal_ref = false) const; // NOLINT(build/unsigned)

  /**
   * @brief      Print status of on-board SFP
   */
  std::string get_sfp_status(uint32_t sfp_id, bool print_out = false) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Control tx laser of on-board SFP softly (I2C command)
   */
  void switch_sfp_soft_tx_control_bit(uint32_t, bool) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Fill hardware monitoring structure.
   */
  void get_info(timinghardwareinfo::TimingTLUMonitorData& mon_data) const;

  /**
   * @brief    Give info to collector.
   */
  void get_info(opmonlib::InfoCollector& ci, int level) const override;

protected:
  const std::vector<std::string> m_dac_devices;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_TLUIONODE_HPP_