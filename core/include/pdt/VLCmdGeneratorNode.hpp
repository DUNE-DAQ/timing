#ifndef __PDT_VLCMDGENERATORNODE_HPP__
#define __PDT_VLCMDGENERATORNODE_HPP__

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"

namespace pdt {

/**
 * @brief      Class for master global node.
 */
class VLCmdGeneratorNode : public TimingNode {
    UHAL_DERIVEDNODE(VLCmdGeneratorNode);
public:
    VLCmdGeneratorNode(const uhal::Node& aNode);
    virtual ~VLCmdGeneratorNode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string getStatus(bool aPrint=false) const override;
    
    /**
     * @brief     Control the endpoint sfp tx laser.
     */
    void switchEndpointSFP(uint32_t aAddr, bool aEnable=false) const;

    /**
     * @brief     Adjust endpoint delay.
     */
    void applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel) const;
};

} // namespace pdt

#endif // __PDT_VLCMDGENERATORNODE_HPP__