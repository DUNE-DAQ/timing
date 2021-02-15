/**
 * @file FLCmdGeneratorNode.hpp
 *
 * FLCmdGeneratorNode is a class providing an interface
 * to the fixed length command generator firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FLCMDGENERATORNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FLCMDGENERATORNODE_HPP_

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "pdt/TimestampGeneratorNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

namespace dunedaq {
namespace pdt {

struct FakeTriggerConfig {
    double mReqRate;
    uint32_t mDiv;
    uint32_t mPS;
    double mActRate;

    FakeTriggerConfig(double aRate) : mReqRate(aRate) {
        // Rate =  (50MHz / 2^(d+8)) / p where n in [0,15] and p in [1,256]
    
        // DIVIDER (int): Frequency divider.
    
        // The division from 50MHz to the desired rate is done in three steps:
        // a) A pre-division by 256 
        // b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
        // c) 1-in-n prescaling set by n = rate_div_p
        
        double lDiv = ceil( log(kSPSClockInHz / (mReqRate * 256 * 256)) / log(2) );
        if (lDiv < 0) {
            mDiv = 0;
        } else if (lDiv > 15) {
            mDiv = 15;
        } else {
            mDiv = lDiv;
        }
        
        uint32_t lPS = (uint32_t) ((kSPSClockInHz / (aRate * 256 * (1 << mDiv))) + 0.5);
        if (lPS  == 0 || lPS > 256) {
            throw BadRequestedFakeTriggerRate(ERS_HERE, "FakeTriggerConfig", aRate, lPS);
        } else {
            mPS = lPS;
        }
        mActRate = (double) kSPSClockInHz / (256 * mPS * (1 << mDiv));
    }

    void print() const {
        ERS_INFO("Requested rate, actual rate: " << mReqRate << ", " << mActRate);
        ERS_INFO("prescale, divisor: " << mPS << ", " << mDiv);
    }
};

/**
 * @brief      Class for master global node.
 */
class FLCmdGeneratorNode : public TimingNode {
    UHAL_DERIVEDNODE(FLCmdGeneratorNode)
public:
    FLCmdGeneratorNode(const uhal::Node& aNode);
    virtual ~FLCmdGeneratorNode();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool aPrint=false) const override;
    
    /**
     * @brief     Send a fixed length command
     */
    void sendFLCmd(uint32_t aCmd, uint32_t aChan, const TimestampGeneratorNode& aTSGen) const;

    /**
     * @brief     Configure fake trigger
     */
    void enableFakeTrigger(uint32_t aChan, uint32_t aDiv, uint32_t aPS, bool aPoisson) const;

    /**
     * @brief     Clear fake trigger configuration
     */
    void disableFakeTrigger(uint32_t aChan) const;

    /**
     * @brief     Get command counters status string
     */
    std::string getCmdCountersTable(bool aPrint=false) const;
};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FLCMDGENERATORNODE_HPP_