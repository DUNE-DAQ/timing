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

namespace pdt {
/**
 * @brief      Base class for timing nodes.
 */
class TimingNode : public uhal::Node {
    
public:
    TimingNode(const uhal::Node& aNode);
    virtual ~TimingNode();

    /**
     * @brief     Get the status string of the timing node. Optionally print it
     */
    virtual std::string getStatus(bool aPrint=false) const = 0;

    /**
     * @brief     Read subnodes.
     */
    std::map<std::string,uhal::ValWord<uint32_t>> readSubNodes(const uhal::Node& aNode, bool dispatch=true) const;

    /**
     * @brief     Reset subnodes.
     */
    void resetSubNodes(const uhal::Node& aNode, uint32_t aValue=0x0, bool dispatch=true) const;

};


} // namespace pdt

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGNODE_HPP_