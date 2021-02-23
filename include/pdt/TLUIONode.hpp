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
#include "pdt/timingmon/Structs.hpp"

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
    const std::vector<std::string> m_dac_devices;
public:
    TLUIONode(const uhal::Node& node);
    virtual ~TLUIONode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string get_status(bool print_out=false) const;
    
    /**
     * @brief      Reset timing node.
     */
    void reset(const std::string& clock_config_file="") const override;

    /**
     * @brief      Configure on-board DAC
     */
    void configure_dac(uint32_t dac_id, uint32_t dac_value, bool internal_ref=false) const;

    /**
     * @brief      Print status of on-board SFP 
     */
    std::string get_sfp_status(uint32_t sfp_id, bool print_out=false) const override;

    /**
     * @brief      Control tx laser of on-board SFP softly (I2C command)
     */
    virtual void switch_sfp_soft_tx_control_bit(uint32_t, bool) const override;

    /**
     * @brief      Fill hardware monitoring structure.
     */
    void get_info(timingmon::TimingTLUMonitorData& mon_data) const;

    /**
     * @brief      Fill extended hardware monitoring structure.
     */
    void get_info(timingmon::TimingTLUMonitorDataDebug& mon_data) const;

};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TLUIONODE_HPP_