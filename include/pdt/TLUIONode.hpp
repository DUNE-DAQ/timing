/**
 * @file TLUIONode.hpp
 *
 * TLUIONode is a class providing an interface
 * to the TLU IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TLUIONODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TLUIONODE_HPP_

// PDT Headers
#include "pdt/IONode.hpp"
#include "TimingIssues.hpp"
#include "timing/timingmon/Structs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>

namespace dunedaq {
namespace pdt {

/**
 * @brief      Class for the TLU board.
 */
class TLUIONode : public IONode {
    UHAL_DERIVEDNODE(TLUIONode)

protected:
    const std::vector<std::string> mDACDevices;
public:
    TLUIONode(const uhal::Node& aNode);
    virtual ~TLUIONode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string get_status(bool aPrint=false) const;
    
    /**
     * @brief      Reset timing node.
     */
    void reset(const std::string& aClockConfigFile="") const override;

    /**
     * @brief      Configure on-board DAC
     */
    void configure_dac(uint32_t aDACId, uint32_t aDACValue, bool aInternalRef=false) const;

    /**
     * @brief      Print status of on-board SFP 
     */
    std::string get_sfp_status(uint32_t aSFPId, bool aPrint=false) const override;

    /**
     * @brief      Control tx laser of on-board SFP softly (I2C command)
     */
    virtual void switch_sfp_soft_tx_control_bit(uint32_t, bool) const override;

    /**
     * @brief      Fill hardware monitoring structure.
     */
    void get_info(timing::timingmon::TimingTLUMonitorData& mon_data) const;

};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TLUIONODE_HPP_