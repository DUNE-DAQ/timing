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
    FanoutIONode(const uhal::Node& aNode, std::string aUIDI2CBus, std::string aUIDI2CDevice, std::string aPLLI2CBus, std::string aPLLI2CDevice, std::vector<std::string> aClockNames, std::vector<std::string> aSFPI2CBuses);
    virtual ~FanoutIONode();

    /**
     * @brief     Reset fanout board
     */
    virtual void reset(int32_t aFanoutMode, const std::string& aClockConfigFile="") const = 0;

    /**
     * @brief     Switch the SFP mux channel
     */
    virtual void switchSFPMUXChannel(uint32_t aSFPID) const = 0;

    /**
     * @brief     Read the active SFP mux channel
     */
    virtual uint32_t readActiveSFPMUXChannel() const = 0;
};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FANOUTIONODE_HPP_