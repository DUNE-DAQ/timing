/**
 * @file KerberosDesign.hpp
 *
 * KerberosDesign is a class providing an interface
 * to the fanout firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_KERBEROSDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_KERBEROSDESIGN_HPP_

// PDT Headers
#include "timing/MasterDesign.hpp"
#include "timing/MasterMuxDesignInterface.hpp"
#include "timing/EndpointDesignInterface.hpp"
#include "timing/CDRMuxDesignInterface.hpp"
#include "timing/TimingSourceMuxDesignInterface.hpp"

#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for timing fanout designs.
 */
class KerberosDesign : public MasterDesign,
public MasterMuxDesignInterface,
public CDRMuxDesignInterface, public TimingSourceMuxDesignInterface, public EndpointDesignInterface
{
  UHAL_DERIVEDNODE(KerberosDesign)
public:
  explicit KerberosDesign(const uhal::Node& node);
  virtual ~KerberosDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  using MasterMuxDesignInterface::measure_endpoint_rtt;
  // /**
  //  * @brief    Give info to collector.
  //  */  
  // void get_info(opmonlib::InfoCollector& ci, int level) const override;

  /**
   * @brief      Prepare the timing fanout for data taking.
   *
   */
  void configure(ClockSource clock_source, TimestampSource ts_source) const override;

  /**
   * @brief      Switch timing source
   *
   */
  void switch_timing_source(ClockSource clock_source) const override;// NOLINT(build/unsigned)
};
} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_KERBEROSDESIGN_HPP_
