/**
 * @file HSINode.hpp
 *
 * HSINode is a class providing an interface
 * to the HSI wrapper firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_HSINODE_HPP_
#define TIMING_INCLUDE_TIMING_HSINODE_HPP_

// PDT Headers
#include "timing/EndpointNode.hpp"
#include "TimingIssues.hpp"
#include "timing/FrequencyCounterNode.hpp"
#include "timing/timingendpointinfo/Structs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for HSI nodes.
 */
class HSINode : public EndpointNode {
    UHAL_DERIVEDNODE(HSINode)
public:
    explicit HSINode(const uhal::Node& node);
    virtual ~HSINode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string get_status(bool print_out=false) const override;

    /**
     * @brief      Enable the endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void enable(uint32_t partition=0, uint32_t address=0) const override;

    /**
     * @brief      Disable the endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void disable() const override;

    /**
     * @brief      Reset the endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void reset(uint32_t partition=0, uint32_t address=0) const override;

    /**
     * @brief      Read the number of words in the data buffer.
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t read_buffer_count() const override;
    
    /**
     * @brief      Read the contents of the endpoint data buffer.
     *
     * @return     { description_of_the_return_value }
     */
    uhal::ValVector< uint32_t > read_data_buffer(uint16_t& n_words, bool read_all=false, bool fail_on_error=false) const;
    uhal::ValVector< uint32_t > read_data_buffer(bool read_all=false, bool fail_on_error=false) const;

    /**
     * @brief      Print the contents of the endpoint data buffer.
     *
     * @return     { description_of_the_return_value }
     */
    std::string get_data_buffer_table(bool read_all=false, bool print_out=false) const override;

    /**
     * @brief     Collect monitoring information for timing endpoint
     *
     */
    //void get_info(timingendpointinfo::TimingEndpointInfo& mon_data) const override;

    /**
     * @brief     Configure HSI triggering
     *
     */
    void configure_hsi(uint32_t src, uint32_t re_mask, uint32_t fe_mask, uint32_t inv_mask, bool dispatch=true) const;

    /**
     * @brief     Start HSI triggering
     *
     */
    void start_hsi(bool dispatch=true) const;

    /**
     * @brief     Stop HSI triggering
     *
     */
    void stop_hsi(bool dispatch=true) const;

    /**
     * @brief     Reset HSI
     *
     */
    void reset_hsi(bool dispatch=true) const;

    /**
     * @brief     Read butffer warning flag
     *
     */
    bool read_buffer_warning() const;

    /**
     * @brief     Read butffer error flag
     *
     */
    bool read_buffer_error() const;

    /**
     * @brief     Read butffer error/warning/word count
     *
     */
    uint32_t read_buffer_state() const;


// Hide methods which do not apply at the moment
private:

    using EndpointNode::read_version;
    using EndpointNode::read_clock_frequency;
    using EndpointNode::read_timestamp;

};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_HSINODE_HPP_