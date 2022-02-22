/**
 * @file OverlordDesignInterface.hpp
 *
 * OverlordDesignInterface is a class providing an interface
 * to the Overlord firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_OVERLORDDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_OVERLORDDESIGNINTERFACE_HPP_

// PDT Headers
#include "timing/FMCIONode.hpp"
#include "timing/MasterDesignInterface.hpp"
#include "timing/PDIMasterNode.hpp"
#include "timing/SIMIONode.hpp"
#include "timing/TLUIONode.hpp"
#include "timing/TriggerReceiverNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include "uhal/Node.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for PDI timing master + trigger receiver endpoint design (known as overlord).
 */
class OverlordDesignInterface : virtual public MasterDesignInterface
{

public:
  explicit OverlordDesignInterface(const uhal::Node& node);
  virtual ~OverlordDesignInterface();

  /**
   * @brief     Reset trigger rx endpoint
   */
  const TriggerReceiverNode& get_external_triggers_endpoint_node() const;

  /**
   * @brief     Reset trigger rx endpoint
   */
  void reset_external_triggers_endpoint() const;

  /**
   * @brief     Enable external triggers
   */
  void enable_external_triggers() const;

  /**
   * @brief     Disable external triggers
   */
  void disable_external_triggers() const;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_OVERLORDDESIGNINTERFACE_HPP_