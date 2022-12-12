/**
 * @file I2C9546SwitchNode.hpp
 *
 * I2CExpanderSlave and I2C9546SwitchNode are classes providing an interface
 * to the I2C expander IC.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_I2C9546SWITCHNODE_HPP_
#define TIMING_INCLUDE_TIMING_I2C9546SWITCHNODE_HPP_

#include "timing/I2CMasterNode.hpp"
#include "timing/I2CSlave.hpp"

#include <map>
#include <string>
#include <vector>

namespace dunedaq {
ERS_DECLARE_ISSUE(timing, I2C9546SwitchSlaveChannelError, " Invalid channel: " << channel, ((std::string)channel))

namespace timing {

/**
 * @class      I2C9546SwitchSlave
 *
 * @brief      I2C slave class to control SFP expander chips.
 * @author     Alessandro Thea
 * @date       April 2018
 */
class I2C9546SwitchSlave : public I2CSlave
{
public:
  I2C9546SwitchSlave(const I2CMasterNode* i2c_master, uint8_t i2c_device_address); // NOLINT(build/unsigned)
  virtual ~I2C9546SwitchSlave();

  /**
   * @brief      Enable channel.
   *
   * @param[in]  channel         Channel identifier
   */
  void enable_channel(uint8_t channel) const; // NOLINT(build/unsigned)

  /**
   * @brief      Enable channel.
   *
   * @param[in]  channel         Channel identifier
   */
  void disable_channel(uint8_t channel) const; // NOLINT(build/unsigned)

  /**
   * @brief      Reads channel states
   *
   * @return     { description_of_the_return_value }
   */
  uint8_t read_channels_states() const; // NOLINT(build/unsigned)

  /**
   * @brief      Set channel statess.
   *
   * @param[in]  channels  Enabled channels byte
   */
  void set_channels_states(uint8_t channels) const; // NOLINT(build/unsigned)

private:
  void ensure_valid_channel(uint8_t channel) const; // NOLINT(build/unsigned)

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_I2C9546SWITCHNODE_HPP_