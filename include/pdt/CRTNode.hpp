#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_CRTNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_CRTNODE_HPP_

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "TimingIssues.hpp"

namespace pdt {

/**
 * @brief      Base class for timing IO nodes.
 */
class CRTNode : public TimingNode {
    UHAL_DERIVEDNODE(CRTNode)
public:
    CRTNode(const uhal::Node& aNode);
    ~CRTNode();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string getStatus(bool aPrint=false) const override;

    /**
     * @brief      Enable the crt endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void enable(uint32_t aPartition, uint32_t aCmd) const;

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
    uint64_t readLastPulseTimestamp() const;

};


} // namespace pdt

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_CRTTNODE_HPP_