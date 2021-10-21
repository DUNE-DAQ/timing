/**
 * @file IONode.hpp
 *
 * IONode is a base class providing an interface
 * to for IO firmware blocks.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_IONODE_HPP_
#define TIMING_INCLUDE_TIMING_IONODE_HPP_

// PDT Headers
#include "timing/TimingNode.hpp"

#include "TimingIssues.hpp"

#include "timing/DACNode.hpp"
#include "timing/FrequencyCounterNode.hpp"
#include "timing/I2CExpanderNode.hpp"
#include "timing/I2CMasterNode.hpp"
#include "timing/I2CSFPNode.hpp"
#include "timing/I2CSlave.hpp"
#include "timing/SI534xNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing IO nodes.
 */
class IONode : public TimingNode
{

public:
  IONode(const uhal::Node& node,
         std::string uid_i2c_bus,
         std::string pll_i2c_bus,
         std::string pll_i2c_device,
         std::vector<std::string> clock_names,
         std::vector<std::string> sfp_i2c_buses);
  virtual ~IONode();

  /**
   * @brief      Get the UID address parameter name.
   *
   * @return     { description_of_the_return_value }
   */
  virtual std::string get_uid_address_parameter_name() const = 0;

  /**
   * @brief      Read the word identifying the timing board.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_board_type() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the word identifying the FPFA carrier board.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_carrier_type() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the word identifying the firmware design in the FPGA.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_design_type() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the word identifying the frequency [units of Hz] of the firmware in the FPGA.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_firmware_frequency() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the word containing the timing board UID.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint64_t read_board_uid() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the word identifying the timing board.
   *
   * @return     { description_of_the_return_value }
   */
  virtual BoardRevision get_board_revision() const;

  /**
   * @brief      Print hardware information
   */
  virtual std::string get_hardware_info(bool print_out = false) const;

  /**
   * @brief      Get the full config path.
   *
   * @return     { description_of_the_return_value }
   */
  virtual std::string get_full_clock_config_file_path(const std::string& clock_config_file, int32_t mode = -1) const;

  /**
   * @brief      Get the an I2C chip.
   *
   * @return     { description_of_the_return_value }
   */
  template<class T>
  std::unique_ptr<const T> get_i2c_device(const std::string& i2c_bus_name, const std::string& i2c_device_name) const;

  /**
   * @brief      Get the PLL chip.
   *
   * @return     { description_of_the_return_value }
   */
  virtual std::unique_ptr<const SI534xSlave> get_pll() const;

  /**
   * @brief      Configure clock chip.
   */
  virtual void configure_pll(const std::string& clock_config_file = "") const;

  /**
   * @brief      Read frequencies of on-board clocks.
   */
  virtual std::vector<double> read_clock_frequencies() const;

  /**
   * @brief      Print frequencies of on-board clocks.
   */
  virtual std::string get_clock_frequencies_table(bool print_out = false) const;

  /**
   * @brief      Print status of on-board PLL.
   */
  virtual std::string get_pll_status(bool print_out = false) const;

  /**
   * @brief      Print status of on-board SFP.
   */
  virtual std::string get_sfp_status(uint32_t sfp_id, bool print_out = false) const; // NOLINT(build/unsigned)

  /**
   * @brief      control tx laser of on-board SFP softly (I2C command)
   */
  virtual void switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const; // NOLINT(build/unsigned)

  /**
   * @brief      Reset timing node.
   */
  virtual void soft_reset() const;

  /**
   * @brief      Reset timing node.
   */
  virtual void reset(const std::string& clock_config_file = "") const = 0;

  /**
   * @brief     Reset fanout board
   */
  virtual void reset(int32_t fanout_mode, // NOLINT(build/unsigned)
                     const std::string& clock_config_file = "") const = 0;

protected:
  const std::string m_uid_i2c_bus;
  const std::string m_pll_i2c_bus;
  const std::string m_pll_i2c_device;
  const std::vector<std::string> m_clock_names;
  const std::vector<std::string> m_sfp_i2c_buses;

  /**
   * @brief      Write soft reset register.
   */
  virtual void write_soft_reset_register() const;
};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/IONode.hxx"

#endif // TIMING_INCLUDE_TIMING_IONODE_HPP_