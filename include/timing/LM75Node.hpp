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

#ifndef TIMING_INCLUDE_TIMING_LM75NODE_HPP_
#define TIMING_INCLUDE_TIMING_LM75NODE_HPP_

#include "timing/I2CMasterNode.hpp"
#include "timing/I2CSlave.hpp"

#include "logging/Logging.hpp" // NOTE: if ISSUES ARE DECLARED BEFORE include logging/Logging.hpp, TLOG_DEBUG<<issue wont work.

#include <map>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

class LM75Node : public I2CSlave
{
public:
  LM75Node(const I2CMasterNode* i2c_master, uint8_t i2c_device_address); // NOLINT(build/unsigned)
  virtual ~LM75Node();

  /**
   * @brief      Read temperature [C].
   *
   */
  float read_temperature() const; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_I2C9546SWITCHNODE_HPP_
