/**
 * @file EndpointDesign.hpp
 *
 * EndpointDesign is a class providing an interface
 * to the endpoint firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_ENDPOINTDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_ENDPOINTDESIGN_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/FMCIONode.hpp"
#include "timing/TopDesign.hpp"

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
template<class IO>
class EndpointDesign : virtual public TopDesign<IO>
{

public:
  explicit EndpointDesign(const uhal::Node& node);
  virtual ~EndpointDesign();

  /**
   * @brief     Get status string, optionally print.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Prepare the timing endpoint for data taking.
   *
   */
  void configure() const;

  /**
   * @brief      Collect operational monitoring information
   */
  template<class T>
  void get_info(T& data) const;

  // In leiu of UHAL_DERIVEDNODE
protected:
  virtual uhal::Node* clone() const;
  //
};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/EndpointDesign.hxx"

#endif // TIMING_INCLUDE_TIMING_ENDPOINTDESIGN_HPP_