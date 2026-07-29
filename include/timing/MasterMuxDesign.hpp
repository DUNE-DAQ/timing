/**
 * @file MasterMuxDesign.hpp
 *
 * MasterMuxDesign is a class providing an interface
 * to the Ouroboros firmware design on a board with MUX.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MASTERMUXDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_MASTERMUXDESIGN_HPP_

// PDT Headers
#include "timing/SFPMuxDesignInterface.hpp"
#include "timing/MasterDesign.hpp"
#include "timing/MasterMuxDesignInterface.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for PDI timing master design on mux board
 */
class MasterMuxDesign
  : public MasterDesign, public SFPMuxDesignInterface, public MasterMuxDesignInterface
{
  UHAL_DERIVEDNODE(MasterMuxDesign)
public:
  explicit MasterMuxDesign(const uhal::Node& node);
  virtual ~MasterMuxDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  using MasterMuxDesignInterface::measure_endpoint_rtt;

  /**
   * @brief      Resync active cdr
   *
   * @return     { description_of_the_return_value }
   */
  void resync_active_cdr() const override; // NOLINT(build/unsigned)

  // In leiu of UHAL_DERIVEDNODE
protected:
 // virtual uhal::Node* clone() const;
  
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MASTERMUXDESIGN_HPP_