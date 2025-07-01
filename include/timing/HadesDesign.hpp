/**
 * @file HadesDesign.hpp
 *
 * HadesDesign is a class providing an interface
 * to top level HSI endpoint firmware designs.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_HADESDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_HADESDESIGN_HPP_

// Timing Headers
#include "timing/ChronosDesign.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <sstream>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for timing master with integrated HSI designs.
 */
class HadesDesign : public ChronosDesign
{
  UHAL_DERIVEDNODE(HadesDesign)
public:
  explicit HadesDesign(const uhal::Node& node);
  virtual ~HadesDesign();

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_HADESDESIGN_HPP_
