/**
 * @file MasterNode.hpp
 *
 * MasterNode is a base class providing an interface
 * for the master type firmware blocks.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERNODE_HPP_

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "TimingIssues.hpp"
#include "pdt/TimestampGeneratorNode.hpp"
#include "pdt/VLCmdGeneratorNode.hpp"
#include "pdt/GlobalNode.hpp"
#include "pdt/EchoMonitorNode.hpp"
#include "pdt/FLCmdGeneratorNode.hpp"
#include "pdt/PartitionNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>

namespace dunedaq {
namespace pdt {

/**
 * @brief      Base class for timing IO nodes.
 */
class MasterNode : public TimingNode {
public:
    MasterNode(const uhal::Node& node);
    virtual ~MasterNode();
    
    /**
     * @brief      Read the current timestamp word.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t read_timestamp() const;

    /**
     * @brief      Set the timestamp to current time.
     */
    virtual void set_timestamp(uint64_t timestamp) const;

    /**
     * @brief     Control the tx line of endpoint sfp
     */
    virtual void switch_endpoint_sfp(uint32_t address, bool turn_on) const = 0;

    /**
     * @brief     Enable RTT endpoint
     */
    virtual void enable_upstream_endpoint() const = 0;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t measure_endpoint_rtt(uint32_t address, bool control_sfp=true) const = 0;
    
    /**
     * @brief     Apply delay to endpoint
     */
    virtual void apply_endpoint_delay(uint32_t address, uint32_t coarse_delay, uint32_t fine_delay, uint32_t phase_delay, bool measure_rtt=false, bool control_sfp=true) const = 0;

    /**
     * @brief     Apply delay to endpoint
     */
    virtual void apply_endpoint_delay(const ActiveEndpointConfig& ept_config, bool measure_rtt=false) const;

    /**
     * @brief     Send a fixed length command
     */
    virtual void send_fl_cmd(uint32_t command, uint32_t channel, uint32_t number_of_commands=1) const = 0;

    /**
     * @brief      Get partition node
     *
     * @return     { description_of_the_return_value }
     */
    virtual const PartitionNode& get_partition_node(uint32_t partition_id) const;
};


} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERNODE_HPP_