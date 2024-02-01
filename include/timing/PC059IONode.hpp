/**
 * @file PC059IONode.hpp
 *
 * PC059IONode is a class providing an interface
 * to the PC059 IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_PC059IONODE_HPP_
#define TIMING_INCLUDE_TIMING_PC059IONODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/SFPMuxIONode.hpp"
#include "timing/timinghardwareinfo/InfoStructs.hpp"
#include "timing/timinghardwareinfo/InfoNljs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include <nlohmann/json.hpp>

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for the PC059 board.
 */
class PC059IONode : public SFPMuxIONode
{
  UHAL_DERIVEDNODE(PC059IONode)

public:
  explicit PC059IONode(const uhal::Node& node);
  virtual ~PC059IONode();

  /**
   * @brief      Get the UID address parameter name.
   *
   * @return     { description_of_the_return_value }
   */
  std::string get_uid_address_parameter_name() const override;
  
  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Reset pc059 node.
   */
  void reset(const std::string& clock_config_file) const override;

  /**
   * @brief      Reset IO, with clock file look up.
   */
  using IONode::reset;

  /**
   * @brief     Switch the SFP mux channel
   */
  void switch_downstream_mux_channel(uint32_t mux_channel) const override; // NOLINT(build/unsigned)

  /**
   * @brief     Read the active SFP mux channel
   */
  uint32_t read_active_downstream_mux_channel() const override; // NOLINT(build/unsigned)

  /**
   * @brief     Switch the SFP I2C mux channel
   */
  void switch_sfp_i2c_mux_channel(uint32_t sfp_id) const; // NOLINT(build/unsigned)

  /**
   * @brief      Print status of on-board SFP.
   */
  std::string get_sfp_status(uint32_t sfp_id, bool print_out = false) const override; // NOLINT(build/unsigned)

  /**
   * @brief      control tx laser of on-board SFP softly (I2C command)
   */
  void switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Fill hardware monitoring structure.
   */
  void get_info(timinghardwareinfo::TimingPC059MonitorData& mon_data) const;

  /**
   * @brief    Give info to collector.
   */
  void get_info(opmonlib::InfoCollector& ci, int level) const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_PC059IONODE_HPP_