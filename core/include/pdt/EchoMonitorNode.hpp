#ifndef __PDT_ECHOMONITORNODE_HPP__
#define __PDT_ECHOMONITORNODE_HPP__

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"

namespace pdt {

/**
 * @brief      Class for master global node.
 */
class EchoMonitorNode : public TimingNode {
    UHAL_DERIVEDNODE(EchoMonitorNode);
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
    std::string getStatus(bool aPrint=false) const override {return "";} // TODO put something here
};

} // namespace pdt

#endif // __PDT_ECHOMONITORNODE_HPP__