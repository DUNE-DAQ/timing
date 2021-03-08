/**
 * @file MasterMuxDesign.hpp
 *
 * MasterDesign is a base class providing an interface
 * to for top level master firmware designs on boards with muxes.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_MASTERMUXDESIGN_HPP_
#define TIMING_INCLUDE_TIMING_MASTERMUXDESIGN_HPP_

// PDT Headers
#include "timing/MasterDesign.hpp"
#include "timing/PC059IONode.hpp"
#include "timing/PDIMasterNode.hpp"

#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>
#include <sstream>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for timing fanout designs.
 */
template <class IO, class MST>
class MasterMuxDesign : public MasterDesign<IO,MST> {

public:
    explicit MasterMuxDesign(const uhal::Node& node);
    virtual ~MasterMuxDesign();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool print_out=false) const override;
    
    /**
     * @brief      Prepare the timing fanout for data taking.
     *
     */
    void configure() const override;

    /**
     * @brief      Reset timing fanout node.
     */
    void reset(const std::string& clock_config_file="") const override;

    /**
     * @brief     Switch the SFP mux channel
     */
    virtual void switch_sfp_mux_channel(uint32_t sfp_id, bool wait_for_rtt_ept_lock) const;

    /**
     * @brief     Read the active SFP mux channel
     */
    virtual uint32_t read_active_sfp_mux_channel() const;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t measure_endpoint_rtt(uint32_t address, bool control_sfp, uint32_t sfp_mux) const;

    /**
     * @brief      Apply delay to endpoint
     */
    virtual void apply_endpoint_delay(uint32_t address, uint32_t coarse_delay, uint32_t fine_delay, uint32_t phase_delay, bool measure_rtt, bool control_sfp, uint32_t sfp_mux) const;

    /**
     * @brief      Apply delay to endpoint
     */
    virtual std::vector<uint32_t> scan_sfp_mux() const;

// In leiu of UHAL_DERIVEDNODE
protected:
    virtual uhal::Node* clone() const;
//
};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/MasterMuxDesign.hxx"

#endif // TIMING_INCLUDE_TIMING_MASTERMUXDESIGN_HPP_