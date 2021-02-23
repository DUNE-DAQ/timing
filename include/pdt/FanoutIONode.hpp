/**
 * @file FanoutIONode.hpp
 *
 * FanoutIONode is a base class providing an interface
 * to fanout IO firmware blocks.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FANOUTIONODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FANOUTIONODE_HPP_

// PDT Headers
#include "pdt/IONode.hpp"
#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>

namespace dunedaq {
namespace pdt {

/**
 * @brief      Base class for the fanout boards.
 */
class FanoutIONode : public IONode {

public:
    FanoutIONode(const uhal::Node& node, std::string uid_i2c_bus, std::string uid_i2c_device, std::string pll_i2c_bus, std::string pll_i2c_device, std::vector<std::string> clock_names, std::vector<std::string> sfp_i2c_buses);
    virtual ~FanoutIONode();

    /**
     * @brief     Reset fanout board
     */
    virtual void reset(int32_t fanout_mode, const std::string& clock_config_file="") const = 0;

    /**
     * @brief     Switch the SFP mux channel
     */
    virtual void switch_sfp_mux_channel(uint32_t sfp_id) const = 0;

    /**
     * @brief     Read the active SFP mux channel
     */
    virtual uint32_t read_active_sfp_mux_channel() const = 0;
};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FANOUTIONODE_HPP_