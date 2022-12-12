/**
 * @file SIMIONode.hpp
 *
 * SIMIONode is a class providing an interface
 * to simulation IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_SIMIONODE_HPP_
#define TIMING_INCLUDE_TIMING_SIMIONODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/IONode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for the timing simulation IO.
 */
class SIMIONode : public IONode
{
  UHAL_DERIVEDNODE(SIMIONode)

public:
  explicit SIMIONode(const uhal::Node& node);
  virtual ~SIMIONode();

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
   * @brief      Read the word identifying the timing board.
   *
   * @return     { description_of_the_return_value }
   */
  BoardRevision get_board_revision() const override;

  /**
   * @brief      Print hardware information
   */
  std::string get_hardware_info(bool print_out = false) const override;

  /**
   * @brief      Reset timing node.
   */
  void reset(const std::string& clock_config_file = "") const override;

  /**
   * @brief     Reset fanout board
   */
  void reset(int32_t fanout_mode = -1, // NOLINT(build/unsigned)
             const std::string& clock_config_file = "") const override;

  /**
   * @brief      Read the word containing the timing board UID.
   *
   * @return     { description_of_the_return_value }
   */
  uint64_t read_board_uid() const override; // NOLINT(build/unsigned)

  /**
   * @brief      Configure clock chip.
   */
  void configure_pll(const std::string& clock_config_file = "") const override;

  /**
   * @brief      Read frequencies of on-board clocks.
   */
  std::vector<double> read_clock_frequencies() const override;

  /**
   * @brief      Print frequencies of on-board clocks.
   */
  std::string get_clock_frequencies_table(bool print_out = false) const override;

  /**
   * @brief      Print status of on-board PLL.
   */
  std::string get_pll_status(bool print_out = false) const override;

  /**
   * @brief      Print status of on-board SFP
   */
  std::string get_sfp_status(uint32_t sfp_id, bool print_out = false) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Control tx laser of on-board SFP softly (I2C command)
   */
  void switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const override; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_SIMIONODE_HPP_