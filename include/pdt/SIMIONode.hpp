/**
 * @file SIMIONode.hpp
 *
 * SIMIONode is a class providing an interface
 * to simulation IO firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SIMIONODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SIMIONODE_HPP_

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
    std::string get_status(bool aPrint=false) const override;

    /**
     * @brief      Read the word identifying the timing board.
     *
     * @return     { description_of_the_return_value }
     */
    BoardRevision get_board_revision() const override;

    /**
     * @brief      Print hardware information
     */
    std::string get_hardware_info(bool aPrint=false) const override;

    /**
     * @brief      Reset timing node.
     */
    void reset(const std::string& aClockConfigFile="") const override;

    /**
     * @brief      Read the word containing the timing board UID.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t read_board_uid() const override;

    /**
     * @brief      Configure clock chip.
     */
    void configure_pll(const std::string& aClockConfigFile="") const override;

    /**
     * @brief      Read frequencies of on-board clocks.
     */
    std::vector<double> read_clock_frequencies() const override;

    /**
     * @brief      Print frequencies of on-board clocks.
     */
    std::string get_clock_frequencies_table(bool aPrint=false) const override;

    /**
     * @brief      Print status of on-board PLL.
     */
    std::string get_pll_status(bool aPrint=false) const override;

    /**
     * @brief      Print status of on-board SFP 
     */
    std::string get_sfp_status(uint32_t aSFPId, bool aPrint=false) const override;

    /**
     * @brief      Control tx laser of on-board SFP softly (I2C command)
     */
    virtual void switch_sfp_soft_tx_control_bit(uint32_t aSFPId, bool aOn) const override;

};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SIMIONode_HPP_