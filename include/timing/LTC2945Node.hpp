/**
 * @file LTC2945Node.hpp
 *
 * LM75Node is a classes providing an interface to the LTC2945 power mon IC.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_LTC2945NODE_HPP_
#define TIMING_INCLUDE_TIMING_LTC2945NODE_HPP_

#include "timing/I2CMasterNode.hpp"
#include "timing/I2CSlave.hpp"

#include "logging/Logging.hpp" // NOTE: if ISSUES ARE DECLARED BEFORE include logging/Logging.hpp, TLOG_DEBUG<<issue wont work.

#include <map>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

class LTC2945Node : public I2CSlave
{
public:
  LTC2945Node(const I2CMasterNode* i2c_master, uint8_t i2c_device_address, double sense_resistance); // NOLINT(build/unsigned)
  virtual ~LTC2945Node();

  /**
   * @brief      Read V in [V].
   *
   */
  double read_v_in() const;

  /**
   * @brief      Read delta sense voltage [V].
   *
   */
  double read_delta_sense_v() const;

  /**
   * @brief      Read power [W].
   *
   */
  double read_power() const;

protected:
  double m_sense_resistance;
private:
  static uint16_t combine_adc_data(uint8_t msb_byte, uint8_t lsb_byte);
  static uint32_t combine_power_data(uint8_t msb_byte_2, uint8_t msb_byte_1, uint8_t lsb_byte);

  constexpr static float v_in_resolution = 0.025;
  constexpr static float delta_sense_v_resolution = 0.000025;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_LTC2945NODE_HPP_