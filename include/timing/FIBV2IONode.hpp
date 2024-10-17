/**
 * @file FIBV2IONode.hpp
 *
 * FMCIONode is a class providing an interface
 * to the FIB IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_FIBV2IONODE_HPP_
#define TIMING_INCLUDE_TIMING_FIBV2IONODE_HPP_

// Timing Headers
#include "timing/SFPMuxIONode.hpp"
#include "timing/TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for the FIB board.
 */
class FIBV2IONode : public IONode {
    UHAL_DERIVEDNODE(FIBV2IONode)

public:
    explicit FIBV2IONode(const uhal::Node& aNode);
    virtual ~FIBV2IONode();
    
    /**
     * @brief      Get the UID address parameter name.
     *
     * @return     { description_of_the_return_value }
     */
    std::string get_uid_address_parameter_name() const override;
    
    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool print_out=false) const override;

    /**
     * @brief      Reset IO, with clock file look up.
     */
    void reset(const ClockSource& clock_source) const override;

    /**
     * @brief      Print status of on-board SFP.
     */
    std::string get_sfp_status(uint32_t sfp_id, bool print_out=false) const override; // NOLINT(build/unsigned)

    /**
     * @brief      control tx laser of on-board SFP softly (I2C command)
     */
    void switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const override; // NOLINT(build/unsigned)

    /**
     * @brief      Switch on or off the SFP tx laser via the I2C IO expander controlling the sfp tx disable pin. aOn=1: laster transmitting, tx disable pin = 0; aOn=0: laster NOT transmitting, tx disable pin = 1. 
     */
    void switch_sfp_tx(uint32_t sfp_id, bool turn_on) const; // NOLINT(build/unsigned)

    // /**
    // * @brief      Fill hardware monitoring structure.
    // */
    // void get_info(timinghardwareinfo::TimingFIBMonitorData& mon_data) const;

    // /**
    //  * @brief    Give info to collector.
    //  */
    // void get_info(opmonlib::InfoCollector& ci, int level) const override;

private:

    void validate_sfp_id(uint32_t sfp_id) const; // NOLINT(build/unsigned)

    /**
     * @brief      No config file for FIB v2
     */
    void reset(const std::string& clock_config_file) const override {}
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_FIBV2IONODE_HPP_
