#ifndef __PDT_FIBIONODE_HPP__
#define __PDT_FIBIONODE_HPP__

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/FanoutIONode.hpp"
#include "pdt/exception.hpp"

namespace pdt {

/**
 * @brief      Class for the FIB board.
 */
class FIBIONode : public FanoutIONode {
    UHAL_DERIVEDNODE(FIBIONode);

public:
    FIBIONode(const uhal::Node& aNode);
    virtual ~FIBIONode();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string getStatus(bool aPrint=false) const override;
    
    /**
     * @brief      Reset FIB node.
     */
    void reset(int32_t aFanoutMode, const std::string& aClockConfigFile="") const override;

    /**
     * @brief      Reset FIB node.
     */
    void reset(const std::string& aClockConfigFile="") const override;

    /**
     * @brief     Switch the SFP mux channel
     */
    void switchSFPMUXChannel(uint32_t aSFPId) const override;

    /**
     * @brief     Read the active SFP mux channel
     */
    uint32_t readActiveSFPMUXChannel() const override;


    /**
     * @brief      Print status of on-board SFP.
     */
    std::string getSFPStatus(uint32_t aSFPId, bool aPrint=false) const override;

    /**
     * @brief      control tx laser of on-board SFP softly (I2C command)
     */
    void switchSFPSoftTxControl(uint32_t aSFPId, bool aOn) const override;

    /**
     * @brief      reset on-board PLL using I2C IO expanders
     */
    void resetPLL() const;

    /**
     * @brief      reset on-board SFP flags using I2C IO expanders
     */
    uint8_t readSFPLOSFlag(uint32_t aSFPId) const;

    /**
     * @brief      reset on-board SFP flags using I2C IO expanders
     */
    uint8_t readSFPFaultFlag(uint32_t aSFPId) const;

    /**
     * @brief      reset on-board SFP flags using I2C IO expanders
     */
    uint8_t readSFPLOSFlags() const;

    /**
     * @brief      reset on-board SFP flags using I2C IO expanders
     */
    uint8_t readSFPFaultFlags() const;

    /**
     * @brief      reset on-board SFP flags using I2C IO expanders
     */
    //void readSFPStatusFlags(uint32_t aSFPId) const;

    /**
     * @brief      Switch on or off the SFP tx laser via the I2C IO expander controlling the sfp tx disable pin. aOn=1: laster transmitting, tx disable pin = 0; aOn=0: laster NOT transmitting, tx disable pin = 1. 
     */
    void switchSFPTx(uint32_t aSFPId, bool aOn) const;



private:

    void validateSFPId(uint32_t aSFPId) const;

};

} // namespace pdt

#endif // __PDT_FIBIONODE_HPP__