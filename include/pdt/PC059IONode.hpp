#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PC059IONODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PC059IONODE_HPP_

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/FanoutIONode.hpp"
#include "TimingIssues.hpp"

namespace pdt {

/**
 * @brief      Class for the PC059 board.
 */
class PC059IONode : public FanoutIONode {
    UHAL_DERIVEDNODE(PC059IONode)

public:
    PC059IONode(const uhal::Node& aNode);
    virtual ~PC059IONode();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string getStatus(bool aPrint=false) const override;
    
    /**
     * @brief      Reset pc059 node.
     */
    void reset(int32_t aFanoutMode, const std::string& aClockConfigFile="") const override;

    /**
     * @brief      Reset pc059 node.
     */
    void reset(const std::string& aClockConfigFile="") const override;

    /**
     * @brief     Switch the SFP mux channel
     */
    void switchSFPMUXChannel(uint32_t aSFPID) const override;

    /**
     * @brief     Read the active SFP mux channel
     */
    uint32_t readActiveSFPMUXChannel() const override;

    /**
     * @brief     Switch the SFP I2C mux channel
     */
    void switchSFPI2CMUXChannel(uint32_t aSFPId) const;

    /**
     * @brief      Print status of on-board SFP.
     */
    std::string getSFPStatus(uint32_t aSFPId, bool aPrint=false) const override;

    /**
     * @brief      control tx laser of on-board SFP softly (I2C command)
     */
    void switchSFPSoftTxControlBit(uint32_t aSFPId, bool aOn) const override;

};

} // namespace pdt

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PC059IONODE_HPP_