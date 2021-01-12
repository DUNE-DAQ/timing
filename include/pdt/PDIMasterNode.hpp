#ifndef __PDT_PDIMASTERNODE_HPP__
#define __PDT_PDIMASTERNODE_HPP__

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/MasterNode.hpp"
#include "pdt/SpillInterfaceNode.hpp"
#include "pdt/TriggerReceiverNode.hpp"
#include "pdt/FLCmdGeneratorNode.hpp"

namespace pdt {

/**
 * @brief      Class for PD-I master timing nodes.
 */
class PDIMasterNode : public MasterNode {
    UHAL_DERIVEDNODE(PDIMasterNode)
public:
    PDIMasterNode(const uhal::Node& aNode);
    virtual ~PDIMasterNode();
    
    /**
     * @brief     Print the status of the timing node.
     */
    std::string getStatus(bool aPrint=false) const override;
    
    /**
     * @brief     Control the tx line of endpoint sfp
     */
    void switchEndpointSFP(uint32_t aAddr, bool aOn) const override;

    /**
     * @brief     Enable RTT endpoint
     */
    bool enableUpstreamEndpoint() const override;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t measureEndpointRTT(uint32_t aAddr, bool aControlSFP=true) const override;

    /**
     * @brief     Apply delay to endpoint
     */
    void applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT=false, bool aControlSFP=true) const override;

    using MasterNode::applyEndpointDelay;
    
    /**
     * @brief     Send a fixed length command
     */
    void sendFLCmd(uint32_t aCmd, uint32_t aChan, uint32_t aNumber=1) const override;

    /**
     * @brief     Configure fake trigger generator
     */
    void enableFakeTrigger(uint32_t aChan, double aRate, bool aPoisson=false) const;

    /**
     * @brief     Clear fake trigger configuration
     */
    void disableFakeTrigger(uint32_t aChan) const;

    /**
     * @brief     Enable spill interface
     */
    void enableSpillInterface() const;

    /**
     * @brief     Configure and enable fake spill generator
     */
    void enableFakeSpills(uint32_t aCycLen=16, uint32_t aSpillLen=8) const;

    /**
     * @brief     Read whether we are in spill or not
     */
    bool readInSpill() const;

    /**
     * @brief     Reset trigger rx endpoint
     */
    void resetExternalTriggersEndpoint() const;

    /**
     * @brief     Enable external triggers
     */
    void enableExternalTriggers() const;

    /**
     * @brief     Disable external triggers
     */
    void disableExternalTriggers() const;

     /**
     * @brief     Retrieve partition node
     */
    const PartitionNode& getPartitionNode(uint32_t aPartID) const;

     /**
     * @brief     Set timestamp to current machine time
     */
    void syncTimestamp() const;
};


} // namespace pdt

#endif // __PDT_PDIMASTERNODE_HPP__