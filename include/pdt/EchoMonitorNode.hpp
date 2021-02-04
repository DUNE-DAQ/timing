#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ECHOMONITORNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ECHOMONITORNODE_HPP_

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"

namespace pdt {

/**
 * @brief      Class for master global node.
 */
class EchoMonitorNode : public TimingNode {
    UHAL_DERIVEDNODE(EchoMonitorNode)
public:
    EchoMonitorNode(const uhal::Node& aNode);
    virtual ~EchoMonitorNode();

    /**
     * @brief      Send echo and measure the round-trip time between master and endpoint
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t sendEchoAndMeasureDelay(int64_t aTimeout=500) const;

    /**
     * @brief     Get status string, optionally print.
     */
    std::string getStatus(bool aPrint=false) const override;
};

} // namespace pdt

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ECHOMONITORNODE_HPP_