/**
 * @file MIBIONode.hpp
 *
 * MIBIONode is a class providing an interface
 * to the FMC IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MIBIONODE_HPP_
#define TIMING_INCLUDE_TIMING_MIBIONODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/SFPMuxIONode.hpp"
#include "timing/I2C9546SwitchNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for the timing FMC board.
 */
class MIBIONode : public SFPMuxIONode
{
  UHAL_DERIVEDNODE(MIBIONode)

public:
  explicit MIBIONode(const uhal::Node& node);
  virtual ~MIBIONode();

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
   * @brief      Configure clock chip.
   */
  void configure_pll(const std::string& clock_config_file = "") const override;
  
  /**
   * @brief      Print status of on-board PLL.
   */
  std::string get_pll_status(bool print_out = false) const override;

  /**
   * @brief      Reset MIB IO.
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
   * @brief     Switch the SFP mux channel
   */
  void switch_upstream_mux_channel(uint32_t mux_channel) const; // NOLINT(build/unsigned)

  /**
   * @brief     Read the active SFP mux channel
   */
  uint32_t read_active_upstream_mux_channel() const; // NOLINT(build/unsigned)

  /**
   * @brief      Print status of on-board SFP.
   */
  std::string get_sfp_status(uint32_t sfp_id, bool print_out = false) const override; // NOLINT(build/unsigned)

  /**
   * @brief      control tx laser of on-board SFP softly (I2C command)
   */
  void switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const override; // NOLINT(build/unsigned)

  // /**
  //  * @brief      Fill hardware monitoring structure.
  //  */
  // void get_info(timinghardwareinfo::TimingMIBMonitorData& mon_data) const;

  // /**
  //  * @brief    Give info to collector.
  //  */
  // void get_info(opmonlib::InfoCollector& ci, int level) const override;

private:
  void validate_sfp_id(uint32_t sfp_id) const; // NOLINT(build/unsigned)
  void validate_amc_slot(uint32_t amc_slot) const; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MIBIONODE_HPP_
