/**
 * @file PhaseMeasurementNode.hpp
 *
 * PhaseMeasurementNode is a class providing an interface
 * to the upstream CDR firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_PHASEMEASUREMENTNODE_HPP_
#define TIMING_INCLUDE_TIMING_PHASEMEASUREMENTNODE_HPP_

// PDT Headers
#include "timing/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for master global node.
 */
class PhaseMeasurementNode : public TimingNode
{
  UHAL_DERIVEDNODE(PhaseMeasurementNode)
public:
  explicit PhaseMeasurementNode(const uhal::Node& node);
  virtual ~PhaseMeasurementNode();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief     Measure phase
   */
  double measure_phase(uint8_t reference_clock=255) const;

  /**
   * @brief     Measure phase
   */
  std::map<uint,double> measure_phases() const;

  /**
   * @brief     Reset
   */
  void reset() const;
protected:
  double calculate_phase(uint reference_clock) const; 
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_UPSTREAMCDRNODE_HPP_