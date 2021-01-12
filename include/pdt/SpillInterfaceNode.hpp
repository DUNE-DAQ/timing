#ifndef __PDT_SPILLINTERFACENODE_HPP__
#define __PDT_SPILLINTERFACENODE_HPP__

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "pdt/TimestampGeneratorNode.hpp"

namespace pdt {

/**
 * @brief      Class for master global node.
 */
class SpillInterfaceNode : public TimingNode {
    UHAL_DERIVEDNODE(SpillInterfaceNode)
public:
    SpillInterfaceNode(const uhal::Node& aNode);
    virtual ~SpillInterfaceNode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string getStatus(bool aPrint=false) const override;
    
    /**
     * @brief     Enable spill interface
     */
    void enable() const;

    /**
     * @brief     Disable interface
     */
    void disable() const;

    /**
     * @brief     Configure and enable fake spill generator
     */
    void enableFakeSpills(uint32_t aCycLen=16, uint32_t aSpillLen=8) const;

    /**
     * @brief     Read whether we are in spill or not
     */
    bool readInSpill() const;

};

} // namespace pdt

#endif // __PDT_SPILLINTERFACENODE_HPP__