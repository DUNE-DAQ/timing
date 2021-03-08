/**
 * @file EndpointNode.hpp
 *
 * EndpointNode is a class providing an interface
 * to the endpoint wrapper firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ENDPOINTNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ENDPOINTNODE_HPP_

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "TimingIssues.hpp"
#include "pdt/FrequencyCounterNode.hpp"
#include "pdt/timingendpointinfo/Structs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace pdt {

/**
 * @brief      Base class for timing IO nodes.
 */
class EndpointNode : public TimingNode {
    UHAL_DERIVEDNODE(EndpointNode)
public:
    explicit EndpointNode(const uhal::Node& node);
    virtual ~EndpointNode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string get_status(bool print_out=false) const override;

    /**
     * @brief      Enable the endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void enable(uint32_t partition=0, uint32_t address=0) const;

    /**
     * @brief      Disable the endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void disable() const;

    /**
     * @brief      Reset the endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void reset(uint32_t partition=0, uint32_t address=0) const;

    /**
     * @brief      Read the current timestamp word.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t read_timestamp() const;

    /**
     * @brief      Read the number of words in the data buffer.
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t read_buffer_count() const;
    
    /**
     * @brief      Read the contents of the endpoint data buffer.
     *
     * @return     { description_of_the_return_value }
     */
    uhal::ValVector< uint32_t > read_data_buffer(bool read_all=false) const;

    /**
     * @brief      Print the contents of the endpoint data buffer.
     *
     * @return     { description_of_the_return_value }
     */
    std::string get_data_buffer_table(bool read_all=false, bool print_out=false) const;

    /**
     * @brief      Read the endpoint clock frequency.
     *
     * @return     { description_of_the_return_value }
     */
    double read_clock_frequency() const;

    /**
     * @brief      Read the endpoint wrapper version
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t read_version() const;

    /**
     * @brief     Collect monitoring information for timing endpoint
     *
     */
    void get_info(timingendpointinfo::TimingEndpointInfo& mon_data) const;
};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ENDPOINTNODE_HPP_