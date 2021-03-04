/**
 * @file SpillInterfaceNode.hpp
 *
 * SpillInterfaceNode is a class providing an interface
 * to spill interface firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SPILLINTERFACENODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SPILLINTERFACENODE_HPP_

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "pdt/TimestampGeneratorNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <string>

namespace dunedaq {
namespace pdt {

/**
 * @brief      Class for master global node.
 */
class SpillInterfaceNode : public TimingNode {
    UHAL_DERIVEDNODE(SpillInterfaceNode)
public:
    explicit SpillInterfaceNode(const uhal::Node& node);
    virtual ~SpillInterfaceNode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string get_status(bool print_out=false) const override;
    
    /**
     * @brief     Enable spill interface
     */
    void enable() const;

    /**
     * @brief     Disable interface
     */
    void disable() const;

    /**
     * @brief     Configure and enable fake spill generator
     */
    void enable_fake_spills(uint32_t cycle_length=16, uint32_t spill_length=8) const;

    /**
     * @brief     Read whether we are in spill or not
     */
    bool read_in_spill() const;

};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SPILLINTERFACENODE_HPP_