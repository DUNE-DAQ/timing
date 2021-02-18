/**
 * @file PDIMasterNode.hpp
 *
 * PDIMasterNode is a class providing an interface
 * to the PD-I master firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PDIMASTERNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PDIMASTERNODE_HPP_

// PDT Headers
#include "pdt/MasterNode.hpp"
#include "pdt/SpillInterfaceNode.hpp"
#include "pdt/TriggerReceiverNode.hpp"
#include "pdt/FLCmdGeneratorNode.hpp"
#include "pdt/timingmon/Structs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>

namespace dunedaq {
namespace pdt {

/**
 * @brief      Class for PD-I master timing nodes.
 */
class PDIMasterNode : public MasterNode {
    UHAL_DERIVEDNODE(PDIMasterNode)
public:
    PDIMasterNode(const uhal::Node& aNode);
    virtual ~PDIMasterNode();
    
    /**
     * @brief     Print the status of the timing node.
     */
    std::string get_status(bool aPrint=false) const override;
    
    /**
     * @brief     Control the tx line of endpoint sfp
     */
    void switch_endpoint_sfp(uint32_t aAddr, bool aOn) const override;

    /**
     * @brief     Enable RTT endpoint
     */
    void enable_upstream_endpoint() const override;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t measureEndpointRTT(uint32_t aAddr, bool aControlSFP=true) const override;

    /**
     * @brief     Apply delay to endpoint
     */
    void apply_endpoint_delay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT=false, bool aControlSFP=true) const override;

    using MasterNode::apply_endpoint_delay;
    
    /**
     * @brief     Send a fixed length command
     */
    void send_fl_cmd(uint32_t aCmd, uint32_t aChan, uint32_t aNumber=1) const override;

    /**
     * @brief     Configure fake trigger generator
     */
    void enable_fake_trigger(uint32_t aChan, double aRate, bool aPoisson=false) const;

    /**
     * @brief     Clear fake trigger configuration
     */
    void disable_fake_trigger(uint32_t aChan) const;

    /**
     * @brief     Enable spill interface
     */
    void enable_spill_interface() const;

    /**
     * @brief     Configure and enable fake spill generator
     */
    void enable_fake_spills(uint32_t aCycLen=16, uint32_t aSpillLen=8) const;

    /**
     * @brief     Read whether we are in spill or not
     */
    bool read_in_spill() const;

    /**
     * @brief     Reset trigger rx endpoint
     */
    void reset_external_triggers_endpoint() const;

    /**
     * @brief     Enable external triggers
     */
    void enable_external_triggers() const;

    /**
     * @brief     Disable external triggers
     */
    void disable_external_triggers() const;

     /**
     * @brief     Retrieve partition node
     */
    const PartitionNode& get_partition_node(uint32_t aPartID) const;

     /**
     * @brief     Set timestamp to current machine time
     */
    void sync_timestamp() const;

    /**
     * @brief     Fill the PD-I master monitoring structure.
     */
    void get_info(timingmon::TimingPDIMasterMonitorData& mon_data) const;
};


} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PDIMASTERNODE_HPP_