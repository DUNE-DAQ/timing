/**
 * @file FIBIONode.hpp
 *
 * FMCIONode is a class providing an interface
 * to the FIB IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_FIBIONODE_HPP_
#define TIMING_INCLUDE_TIMING_FIBIONODE_HPP_

// Timing Headers
#include "timing/FanoutIONode.hpp"
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
class FIBIONode : public FanoutIONode {
    UHAL_DERIVEDNODE(FIBIONode)

public:
    explicit FIBIONode(const uhal::Node& aNode);
    virtual ~FIBIONode();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool print_out=false) const override;
    
    /**
     * @brief      Reset FIB node.
     */
    void reset(int32_t fanout_mode, const std::string& clock_config_file="") const override;

    /**
     * @brief      Reset FIB node.
     */
    void reset(const std::string& clock_config_file="") const override;

    /**
     * @brief     Switch the SFP mux channel
     */
    void switch_sfp_mux_channel(uint32_t sfp_id) const override; // NOLINT(build/unsigned)

    /**
     * @brief     Read the active SFP mux channel
     */
    uint32_t read_active_sfp_mux_channel() const override; // NOLINT(build/unsigned)


    /**
     * @brief      Print status of on-board SFP.
     */
    std::string get_sfp_status(uint32_t sfp_id, bool print_out=false) const override; // NOLINT(build/unsigned)

    /**
     * @brief      control tx laser of on-board SFP softly (I2C command)
     */
    void switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const override; // NOLINT(build/unsigned)

    /**
     * @brief      reset on-board PLL using I2C IO expanders
     */
    void reset_pll() const;

    /**
     * @brief      reset on-board SFP flags using I2C IO expanders
     */
    uint8_t read_sfp_los_flag(uint32_t sfp_id) const; // NOLINT(build/unsigned)

    /**
     * @brief      reset on-board SFP flags using I2C IO expanders
     */
    uint8_t read_sfp_fault_flag(uint32_t sfp_id) const; // NOLINT(build/unsigned)

    /**
     * @brief      reset on-board SFP flags using I2C IO expanders
     */
    uint8_t read_sfp_los_flags() const; // NOLINT(build/unsigned)

    /**
     * @brief      reset on-board SFP flags using I2C IO expanders
     */
    uint8_t read_sfp_fault_flags() const; // NOLINT(build/unsigned)

    /**
     * @brief      reset on-board SFP flags using I2C IO expanders
     */
    //void readSFPStatusFlags(uint32_t aSFPId) const;

    /**
     * @brief      Switch on or off the SFP tx laser via the I2C IO expander controlling the sfp tx disable pin. aOn=1: laster transmitting, tx disable pin = 0; aOn=0: laster NOT transmitting, tx disable pin = 1. 
     */
    void switch_sfp_tx(uint32_t sfp_id, bool turn_on) const; // NOLINT(build/unsigned)



private:

    void validate_sfp_id(uint32_t sfp_id) const; // NOLINT(build/unsigned)

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_FIBIONODE_HPP_