/**
 * @file CRTNode.hpp
 *
 * CRTNode is a class providing an interface
 * to the crt wrapper firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_CRTNODE_HPP_
#define TIMING_INCLUDE_TIMING_CRTNODE_HPP_

#include "timing/TimingNode.hpp"
#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing IO nodes.
 */
class CRTNode : public TimingNode {
    UHAL_DERIVEDNODE(CRTNode)
public:
    explicit CRTNode(const uhal::Node& node);
    ~CRTNode();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool print_out=false) const override;

    /**
     * @brief      Enable the crt endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void enable(uint32_t partition, uint32_t command) const;

    /**
     * @brief      Disable the crt endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void disable() const;

    /**
     * @brief      Read the timestamp of the last pulse.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t read_last_pulse_timestamp() const;

};

} // namespace timing
} // namespace dunedaq

#endif  // TIMING_INCLUDE_TIMING_CRTNODE_HPP_