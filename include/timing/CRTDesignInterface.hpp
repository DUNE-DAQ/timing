/**
 * @file CRTDesignInterface.hpp
 *
 * CRTDesignInterface is a class providing an interface
 * to the endpoint firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_CRTDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_CRTDESIGNINTERFACE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/CRTNode.hpp"
#include "timing/EndpointDesignInterface.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing hsi design nodes.
 */
class CRTDesignInterface : virtual public EndpointDesignInterface
{

public:
  explicit CRTDesignInterface(const uhal::Node& node) 
    : EndpointDesignInterface(node) 
    {}
  virtual ~CRTDesignInterface() {}

  /**
   * @brief      Get the CRT node.
   *
   * @return     { description_of_the_return_value }
   */
  virtual const CRTNode& get_crt_node() const { return uhal::Node::getNode<CRTNode>("endpoint0"); }

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_CRTDESIGNINTERFACE_HPP_