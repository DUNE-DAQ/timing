/**
 * @file PC059IONode.hpp
 *
 * PC059IONode is a class providing an interface
 * to the PC059 IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PC059IONODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PC059IONODE_HPP_

// PDT Headers
#include "pdt/FanoutIONode.hpp"
#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace pdt {

/**
 * @brief      Class for the PC059 board.
 */
class PC059IONode : public FanoutIONode {
    UHAL_DERIVEDNODE(PC059IONode)

public:
    explicit PC059IONode(const uhal::Node& node);
    virtual ~PC059IONode();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool print_out=false) const override;
    
    /**
     * @brief      Reset pc059 node.
     */
    void reset(int32_t fanout_mode, const std::string& clock_config_file="") const override;

    /**
     * @brief      Reset pc059 node.
     */
    void reset(const std::string& clock_config_file="") const override;

    /**
     * @brief     Switch the SFP mux channel
     */
    void switch_sfp_mux_channel(uint32_t sfp_id) const override;

    /**
     * @brief     Read the active SFP mux channel
     */
    uint32_t read_active_sfp_mux_channel() const override;

    /**
     * @brief     Switch the SFP I2C mux channel
     */
    void switch_sfp_i2c_mux_channel(uint32_t sfp_id) const;

    /**
     * @brief      Print status of on-board SFP.
     */
    std::string get_sfp_status(uint32_t sfp_id, bool print_out=false) const override;

    /**
     * @brief      control tx laser of on-board SFP softly (I2C command)
     */
    void switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const override;

};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PC059IONODE_HPP_