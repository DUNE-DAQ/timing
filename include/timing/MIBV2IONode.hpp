/**
 * @file MIBV2IONode.hpp
 *
 * MIBV2IONode is a class providing an interface
 * to the MIB V2 IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MIBV2IONODE_HPP_
#define TIMING_INCLUDE_TIMING_MIBV2IONODE_HPP_

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

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for the timing FMC board.
 */
class MIBV2IONode : public IONode
{
  UHAL_DERIVEDNODE(MIBV2IONode)

public:
  explicit MIBV2IONode(const uhal::Node& node);
  virtual ~MIBV2IONode();

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
   * @brief      Reset MIB v2 IO.
   */
  void reset(const std::string& clock_config_file) const override;

  /**
   * @brief      Reset IO, with clock file look up.
   */
  void reset(const ClockSource& clock_source) const override;

  /**
   * @brief      Switch clock input, with clock file look up and upload if necessary
   */
  void switch_clock_source(const ClockSource& clock_source) const;

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
  //void get_info(timinghardwareinfo::TimingMIBV2MonitorData& mon_data) const;

  /**
   * @brief    Give info to collector.
   */
  void get_info(opmonlib::InfoCollector& ci, int level) const override;

private:
  void validate_sfp_id(uint32_t sfp_id) const; // NOLINT(build/unsigned)
  void validate_amc_slot(uint32_t amc_slot) const; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MIBV2IONODE_HPP_