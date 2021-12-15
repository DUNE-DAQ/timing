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
  explicit EndpointDesignInterface(const uhal::Node& node) 
    : TopDesignInterface(node) {}
  virtual ~EndpointDesignInterface() {}

  /**
   * @brief      Return the timing endpoint node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const EndpointNodeInterface* get_endpoint_node_plain(uint32_t ept_id) const // NOLINT(build/unsigned)
  {
    const std::string node_name = "endpoint" + std::to_string(ept_id);
    return dynamic_cast<const EndpointNodeInterface*>(&uhal::Node::getNode(node_name)); 
  }

  /**
   * @brief      Return the timing endpoint node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t get_number_of_endpoint_nodes() const // NOLINT(build/unsigned)
  {
    std::string regex_string = "endpoint[0-9]+";
    return uhal::Node::getNodes(regex_string).size();
  }
};

/**
 * @brief      Base class for plain timing endpoint design nodes.
 */
class PlainEndpointDesignInterface : virtual public EndpointDesignInterface
{

public:
  explicit PlainEndpointDesignInterface(const uhal::Node& node)
    : EndpointDesignInterface(node) {}
  virtual ~PlainEndpointDesignInterface() {}

  /**
   * @brief      Return the timing endpoint node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const EndpointNode& get_endpoint_node(uint32_t ept_id) const // NOLINT(build/unsigned)
  {
    return dynamic_cast<const EndpointNode&>(*get_endpoint_node_plain(ept_id));
  }
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_ENDPOINTDESIGNINTERFACE_HPP_