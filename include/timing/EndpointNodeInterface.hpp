/**
 * @file EndpointNodeInterface.hpp
 *
 * EndpointNodeInterface is a class providing an interface
 * to the endpoint wrapper firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_ENDPOINTNODEINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_ENDPOINTNODEINTERFACE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing IO nodes.
 */
class EndpointNodeInterface : public TimingNode
{
public:
  explicit EndpointNodeInterface(const uhal::Node& node);
  virtual ~EndpointNodeInterface();

  /**
   * @brief      Enable the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  virtual void enable(uint32_t address = 0, uint32_t partition = 0) const = 0; // NOLINT(build/unsigned)

  /**
   * @brief      Disable the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  virtual void disable() const = 0;

  /**
   * @brief      Reset the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  virtual void reset(uint32_t address = 0, uint32_t partition = 0) const = 0; // NOLINT(build/unsigned)

  /**
   * @brief      Get endpoint ready flag
   *
   * @return     { description_of_the_return_value }
   */
  virtual bool endpoint_ready() const; // NOLINT(build/unsigned)

  /**
   * @brief      Get endpoint state
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_endpoint_state() const; // NOLINT(build/unsigned)

};

} // namespace timing
} // namespace dunedaq

#endif  // TIMING_INCLUDE_TIMING_ENDPOINTNODEINTERFACE_HPP_