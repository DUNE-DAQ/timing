#ifndef __PDT_TLUIONODE_HPP__
#define __PDT_TLUIONODE_HPP__

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/IONode.hpp"
#include "pdt/exception.hpp"

namespace pdt {

/**
 * @brief      Class for the TLU board.
 */
class TLUIONode : public IONode {
    UHAL_DERIVEDNODE(TLUIONode);

protected:
    const std::vector<std::string> mDACDevices;
public:
    TLUIONode(const uhal::Node& aNode);
    virtual ~TLUIONode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string getStatus(bool aPrint=false) const;
    
    /**
     * @brief      Reset timing node.
     */
    void reset(const std::string& aClockConfigFile="") const override;

    /**
     * @brief      Configure on-board DAC
     */
    void configureDAC(uint32_t aDACId, uint32_t aDACValue, bool aInternalRef=false) const;

    /**
     * @brief      Print status of on-board SFP 
     */
    std::string getSFPStatus(uint32_t aSFPId, bool aPrint=false) const override;

    /**
     * @brief      Control tx laser of on-board SFP softly (I2C command)
     */
    virtual void switchSFPSoftTxControlBit(uint32_t aSFPId, bool aOn) const override;

};

} // namespace pdt

#endif // __PDT_TLUIONODE_HPP__