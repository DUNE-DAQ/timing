/**
 * @file MasterMuxDesign.hpp
 *
 * MasterMuxDesign is a class providing an interface
 * to the master type firmware design with MUX.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MASTERMUXDESIGNINTERFACE_HPP_
#define TIMING_INCLUDE_TIMING_MASTERMUXDESIGNINTERFACE_HPP_

// PDT Headers
#include "timing/MuxDesignInterface.hpp"
#include "timing/MasterDesignInterface.hpp"

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
class MasterMuxDesignInterface
  : virtual public MuxDesignInterface, virtual public MasterDesignInterface
{
public:
  explicit MasterMuxDesignInterface(const uhal::Node& node)
  : MuxDesignInterface(node)
  , MasterDesignInterface(node) {}
  virtual ~MasterMuxDesignInterface() {}
  
  /**
   * @brief      Measure the endpoint round trip time.
   *
   * @return     { description_of_the_return_value }
   */
    virtual uint32_t measure_endpoint_rtt(uint16_t address, // NOLINT(build/unsigned)
                                          uint8_t master_mux,
                                          bool control_sfp = true) const
    {
        switch_mux(master_mux);
        return dynamic_cast<const MasterDesignInterface*>(this)->measure_endpoint_rtt(address, control_sfp);
    }

  /**
   * @brief      Measure the endpoint round trip time.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t measure_endpoint_rtt(uint16_t address, // NOLINT(build/unsigned)
                                        uint16_t fanout_endpoint_address,
                                        uint8_t fanout_mux,
                                        uint8_t master_mux,
                                        bool control_sfp = true) const
    {
      switch_mux(master_mux);
      return dynamic_cast<const MasterDesignInterface*>(this)->measure_endpoint_rtt(address, fanout_endpoint_address, fanout_mux, control_sfp);
    }
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_MASTERMUXDESIGNINTERFACE_HPP_