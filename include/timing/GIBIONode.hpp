/**
 * @file GIBIONode.hpp
 *
 * GIBIONode is a class providing an interface
 * to the MIB V2 IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_GIBIONODE_HPP_
#define TIMING_INCLUDE_TIMING_GIBIONODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/IONode.hpp"

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
class GIBIONode : public IONode
{
  UHAL_DERIVEDNODE(GIBIONode)

public:
  explicit GIBIONode(const uhal::Node& node);
  explicit GIBIONode(const uhal::Node& node,
                     std::string uid_i2c_bus,
                     std::string pll_i2c_bus,
                     std::string pll_i2c_device,
                     std::vector<std::string> clock_names,
                     std::vector<std::string> sfp_i2c_buses);
  virtual ~GIBIONode();

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
   * @brief      GET PLL I2C interface.
   */
  std::unique_ptr<const SI534xSlave> get_pll() const override;

  /**
   * @brief      Print hardware information
   */
  std::string get_hardware_info(bool print_out) const override;
  
  /**
   * @brief      Set up i2c buses, enable ICs.
   */
  void set_up_io_infrastructure() const override;

  /**
   * @brief      Reset GIB IO.
   */
  void reset(const std::string& clock_config_file)  const override;

  /**
   * @brief      Configure the GIB expander.
   */
  void configure_expander()  const;

  /**
   * @brief     Reset timing node with clock file lookup
   */
  using IONode::reset;

  /**
   * @brief      Reset PLL.
   */
  void reset_pll()  const override;

  /**
   * @brief      Print status of on-board SFP.
   */
  std::string get_sfp_status(uint32_t sfp_id, bool print_out = false) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Read the contents of the IO expanders.
   */
  virtual uint32_t read_io_expanders() const; // NOLINT(build/unsigned)

  /**
   * @brief      Retrive SFP LOS status for all SFPs.
   */
  virtual uint8_t read_sfps_los() const; // NOLINT(build/unsigned)

  /**
   * @brief      Retrive SFP fault status for all SFPs.
   */
  virtual uint8_t read_sfps_fault() const; // NOLINT(build/unsigned)

  /**
   * @brief      control tx laser of on-board SFP softly (I2C command)
   */
  void switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const override; // NOLINT(build/unsigned)

  /**
   * @brief     Clocks ready?
   */
  bool clocks_ok() const override;

  /**
   * @brief      control tx laser of on-board SFP softly (I2C command)
   */
  void switch_sfp_tx(uint32_t sfp_id, bool turn_on) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Fill hardware monitoring structure.
   */
  //void get_info(timinghardwareinfo::TimingGIBMonitorData& mon_data) const;

  // /**
  //  * @brief    Give info to collector.
  //  */
  // void get_info(opmonlib::InfoCollector& ci, int level) const override;

  /**
   * @brief    Configure which mux channels are on using a bitmask
   */
  void set_i2c_mux_channels(uint8_t mux_channel_bitmask) const;

  /**
   * @brief    Read board temp
   */
  float read_board_temperature() const;

protected:
  static const uint8_t num_sfps = 6;

  static const uint8_t sfp_tx_disable_bitmap = 0xC0;

  void validate_sfp_id(uint32_t sfp_id) const; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_GIBIONODE_HPP_
