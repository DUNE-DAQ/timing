#ifndef __PDT_SIMIONODE_HPP__
#define __PDT_SIMIONODE_HPP__

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/IONode.hpp"
#include "pdt/exception.hpp"

namespace pdt {

/**
 * @brief      Class for the timing simulation IO.
 */
class SIMIONode : public IONode {
    UHAL_DERIVEDNODE(SIMIONode)

public:
    SIMIONode(const uhal::Node& aNode);
    virtual ~SIMIONode();
    
    /**
     * @brief     Get status string, optionally print.
     */
    std::string getStatus(bool aPrint=false) const override;

    /**
     * @brief      Read the word identifying the timing board.
     *
     * @return     { description_of_the_return_value }
     */
    BoardRevision getBoardRevision() const override;

    /**
     * @brief      Print hardware information
     */
    std::string getHardwareInfo(bool aPrint=false) const override;

    /**
     * @brief      Reset timing node.
     */
    void reset(const std::string& aClockConfigFile="") const override;

    /**
     * @brief      Read the word containing the timing board UID.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t readBoardUID() const override;

    /**
     * @brief      Configure clock chip.
     */
    void configurePLL(const std::string& aClockConfigFile="") const override;

    /**
     * @brief      Read frequencies of on-board clocks.
     */
    std::vector<double> readClockFrequencies() const override;

    /**
     * @brief      Print frequencies of on-board clocks.
     */
    std::string getClockFrequenciesTable(bool aPrint=false) const override;

    /**
     * @brief      Print status of on-board PLL.
     */
    std::string getPLLStatus(bool aPrint=false) const override;

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

#endif // __PDT_SIMIONode_HPP__