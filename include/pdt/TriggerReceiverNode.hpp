/**
 * @file TriggerReceiverNode.hpp
 *
 * TriggerReceiverNode is a class providing an interface
 * to the timing trigger receiver node.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TRIGGERRECEIVERNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TRIGGERRECEIVERNODE_HPP_

// PDT Headers
#include "pdt/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

namespace dunedaq {
namespace pdt {

/**
 * @brief      Class for master global node.
 */
class TriggerReceiverNode : public TimingNode {
    UHAL_DERIVEDNODE(TriggerReceiverNode)
public:
    TriggerReceiverNode(const uhal::Node& aNode);
    virtual ~TriggerReceiverNode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string get_status(bool aPrint=false) const override;
    
    /**
     * @brief     Enable trigger rx endpoint
     */
    void enable() const;

    /**
     * @brief     Disable trigger rx endpoint
     */
    void disable() const;

    /**
     * @brief     Reset trigger rx endpoint
     */
    void reset() const;

    /**
     * @brief     Enable external triggers
     */
    void enable_triggers() const;

    /**
     * @brief     Disable external triggers
     */
    void disable_triggers() const;

};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TRIGGERRECEIVERNODE_HPP_