#ifndef __PDT_GLOBALNODE_HPP__
#define __PDT_GLOBALNODE_HPP__

// uHal Headers
#include "uhal/DerivedNode.hpp"

namespace pdt {

/**
 * @brief      Class for partition node.
 */
class GlobalNode : public uhal::Node {
    UHAL_DERIVEDNODE(GlobalNode);
public:
    GlobalNode(const uhal::Node& aNode);
    virtual ~GlobalNode();

    bool inSpill() const;
    bool txError() const;

    uint32_t readTimeStamp() const;
    uint32_t readSpillCounter() const;

    void selectPartition() const;
    void lockPartition() const;

};

} // namespace pdt

#endif // __PDT_GLOBALNODE_HPP__