/**
 * @file EndpointDesignInterface.hpp
 *
 * EndpointDesignInterface is a class providing an interface
 * to the endpoint firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_ENDPOINTDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_ENDPOINTDESIGNINTERFACE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/TopDesignInterface.hpp"
#include "timing/EndpointNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing endpoint design nodes.
 */
class EndpointDesignInterface : virtual public TopDesignInterface
{

public:
  explicit EndpointDesignInterface(const uhal::Node& node);
  virtual ~EndpointDesignInterface();

  /**
   * @brief      Return the timing endpoint node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const EndpointNode& get_endpoint_node(uint32_t ept_id) const; // NOLINT(build/unsigned)

  /**
   * @brief      Return the timing endpoint node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t get_number_of_endpoint_nodes() const; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_ENDPOINTDESIGNINTERFACE_HPP_