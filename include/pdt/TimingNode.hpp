/**
 * @file TimingNode.hpp
 *
 * TimingNode is a base class for timing node
 * inteface classes.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGNODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "pdt/definitions.hpp"
#include "pdt/toolbox.hpp"
#include "ers/Issue.h"
#include "ers/ers.h"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>

namespace dunedaq {
namespace pdt {
/**
 * @brief      Base class for timing nodes.
 */
class TimingNode : public uhal::Node {
    
public:
    TimingNode(const uhal::Node& node);
    virtual ~TimingNode();

    /**
     * @brief     Get the status string of the timing node. Optionally print it
     */
    virtual std::string get_status(bool print_out=false) const = 0;

    /**
     * @brief     Read subnodes.
     */
    std::map<std::string,uhal::ValWord<uint32_t>> read_sub_nodes(const uhal::Node& node, bool dispatch=true) const;

    /**
     * @brief     Reset subnodes.
     */
    void reset_sub_nodes(const uhal::Node& node, uint32_t aValue=0x0, bool dispatch=true) const;
};


} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGNODE_HPP_