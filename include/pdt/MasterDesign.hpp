/**
 * @file MasterDesign.hpp
 *
 * MasterDesign is a base class providing an interface
 * to for top level master firmware designs.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERDESIGN_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERDESIGN_HPP_

// PDT Headers
#include "pdt/TopDesign.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>
#include <sstream>

namespace dunedaq {
namespace pdt {

/**
 * @brief      Base class for timing master designs.
 */
template <class IO, class MST>
class MasterDesign : public TopDesign<IO> {

public:
    MasterDesign(const uhal::Node& node);
    virtual ~MasterDesign();

    /**
     * @brief      Get the timing master node.
     *
     * @return     { description_of_the_return_value }
     */
    virtual const MST& get_master_node() const;

    /**
     * @brief      Configure the timing master design node.
     */
    virtual void configure() const = 0;

    /**
     * @brief      Read the current timestamp.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t read_master_timestamp() const;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t measure_endpoint_rtt(uint32_t address, bool control_sfp=true) const;

    /**
     * @brief      Apply delay to endpoint
     */
    virtual void apply_endpoint_delay(uint32_t address, uint32_t coarse_delay, uint32_t fine_delay, uint32_t phase_delay, bool measure_rtt=false, bool control_sfp=true) const;

};

} // namespace pdt
} // namespace dunedaq

#include "pdt/detail/MasterDesign.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERDESIGN_HPP_