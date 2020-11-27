#ifndef __PDT_FANOUTIONODE_HPP__
#define __PDT_FANOUTIONODE_HPP__

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/IONode.hpp"
#include "pdt/exception.hpp"

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

#endif // __PDT_FANOUTIONODE_HPP__