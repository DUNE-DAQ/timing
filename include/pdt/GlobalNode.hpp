#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_GLOBALNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_GLOBALNODE_HPP_

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"

namespace pdt {

/**
 * @brief      Class for master global node.
 */
class GlobalNode : public TimingNode {
    UHAL_DERIVEDNODE(GlobalNode)
public:
    GlobalNode(const uhal::Node& aNode);
    virtual ~GlobalNode();

    bool inSpill() const;
    bool txError() const;

    uint32_t readTimeStamp() const;
    uint32_t readSpillCounter() const;

    void selectPartition() const;
    void lockPartition() const;

    /**
     * @brief     Enable the upstream endpoint.
     */
    void enableUpstreamEndpoint(uint32_t aTimeout=500);

    /**
     * @brief     Get status string, optionally print.
     */
    std::string getStatus(bool aPrint=false) const override;
};

} // namespace pdt

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_GLOBALNODE_HPP_