#ifndef __PDT_SI5344_HPP__
#define __PDT_SI5344_HPP__

// uHal Headers
#include "uhal/DerivedNode.hpp"

namespace pdt {

/**
 * @brief      Class for partition node.
 */
class PartitionNode : public uhal::Node {
    UHAL_DERIVEDNODE(PartitionNode);
public:
    PartitionNode(const uhal::Node& aNode);
    virtual ~PartitionNode();

    /**
     * @brief      Enables the partition now.
     *
     * @param[in]  aEnable  A enable
     */
    void enableNow( bool aEnable=true ) const;

    /**
     * @brief      Resets the partition
     */
    void reset() const;

    void start() const;

    void stop() const;


};

} // namespace pdt

#endif