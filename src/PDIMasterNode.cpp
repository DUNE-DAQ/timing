#include "pdt/PDIMasterNode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(PDIMasterNode)

//-----------------------------------------------------------------------------
PDIMasterNode::PDIMasterNode(const uhal::Node& aNode) : MasterNode(aNode) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
PDIMasterNode::~PDIMasterNode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
PDIMasterNode::getStatus(bool aPrint) const {
    std::stringstream lStatus;
    auto lTStamp = getNode<TimestampGeneratorNode>("master.tstamp").readRawTimestamp();
    lStatus << "Timestamp: 0x" << std::hex << tstamp2int(lTStamp) << " -> " << formatTimestamp(lTStamp) << std::endl << std::endl;
    lStatus << getNode<FLCmdGeneratorNode>("master.scmd_gen").getCmdCountersTable();
    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::switchEndpointSFP(uint32_t aAddr, bool aOn) const {
    auto vlCmdNode = getNode<VLCmdGeneratorNode>("master.acmd");

    // Switch off endpoint SFP tx
    vlCmdNode.switchEndpointSFP(aAddr, aOn);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
PDIMasterNode::enableUpstreamEndpoint() const {
    auto lGlobal = getNode<GlobalNode>("master.global");
    return lGlobal.enableUpstreamEndpoint();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
PDIMasterNode::measureEndpointRTT(uint32_t aAddr, bool aControlSFP) const {

    auto vlCmdNode = getNode<VLCmdGeneratorNode>("master.acmd");
    auto lGlobal = getNode<GlobalNode>("master.global");
    auto lEcho = getNode<EchoMonitorNode>("master.echo");

    if (aControlSFP) {
        // Switch off all TX SFPs
        vlCmdNode.switchEndpointSFP(0x0, false);
    
        // Turn on the current target
        vlCmdNode.switchEndpointSFP(aAddr, true);
    
        millisleep(100);
    }

    if (!lGlobal.enableUpstreamEndpoint()) {
        std::ostringstream lMsg;
        lMsg << "RTT endpoint failed to lock. Downstream ept address: " << formatRegValue(aAddr);
        throw UpstreamEndpointFailedToLock(lMsg.str());
    }
    
    uint32_t lEndpointRTT = lEcho.sendEchoAndMeasureDelay();
    
    if (aControlSFP) vlCmdNode.switchEndpointSFP(aAddr, false);

    return lEndpointRTT;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT, bool aControlSFP) const {
    
    auto vlCmdNode = getNode<VLCmdGeneratorNode>("master.acmd");
    auto lGlobal = getNode<GlobalNode>("master.global");
    auto lEcho = getNode<EchoMonitorNode>("master.echo");

    if (aMeasureRTT) {
        
        if (aControlSFP) {
            // Switch off all TX SFPs
            vlCmdNode.switchEndpointSFP(0x0, false);
            
            // Turn on the current target
            vlCmdNode.switchEndpointSFP(aAddr, true);
        
            millisleep(100);
        }

        if (!lGlobal.enableUpstreamEndpoint()) {
            std::ostringstream lMsg;
            lMsg << "RTT endpoint failed to lock. Downstream ept address: " << formatRegValue(aAddr);
            throw UpstreamEndpointFailedToLock(lMsg.str());
        }

        uint64_t lEndpointRTT = lEcho.sendEchoAndMeasureDelay();
        PDT_LOG(kNotice) << "Pre delay adjustment RTT:  " << std::dec << lEndpointRTT; 
    }
    
    vlCmdNode.applyEndpointDelay(aAddr, aCDel, aFDel, aPDel);

    millisleep(100);

    if (aMeasureRTT) {
        if (!lGlobal.enableUpstreamEndpoint()) {
            std::ostringstream lMsg;
            lMsg << "RTT endpoint failed to lock. Downstream ept address: " << formatRegValue(aAddr);
            throw UpstreamEndpointFailedToLock(lMsg.str());
        }

        uint64_t lEndpointRTT = lEcho.sendEchoAndMeasureDelay();
        PDT_LOG(kNotice) << "Post delay adjustment RTT: " << std::dec << lEndpointRTT << std::endl;

        if (aControlSFP) vlCmdNode.switchEndpointSFP(aAddr, false);
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::sendFLCmd(uint32_t aCmd, uint32_t aChan, uint32_t aNumber) const {
    for (uint32_t i=0; i < aNumber; i++) {
        getNode<FLCmdGeneratorNode>("master.scmd_gen").sendFLCmd(aCmd, aChan, getNode<TimestampGeneratorNode>("master.tstamp"));
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::enableFakeTrigger(uint32_t aChan, double aRate, bool aPoisson) const {

    // Configures the internal command generator to produce triggers at a defined frequency.
    // Rate =  (50MHz / 2^(d+8)) / p where n in [0,15] and p in [1,256]

    // DIVIDER (int): Frequency divider.
    

    // The division from 50MHz to the desired rate is done in three steps:
    // a) A pre-division by 256
    // b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
    // c) 1-in-n prescaling set by n = rate_div_p
    
    FakeTriggerConfig lFTConfig(aRate);

    lFTConfig.print();
    PDT_LOG(kNotice) << "> Trigger rate for FakeTrig" << aChan << " (0x" << std::hex << 0x8+aChan << ") set to " << std::setprecision(3) << std::scientific << lFTConfig.mActRate << " Hz";
    std::stringstream lTriggerModeStream;
    lTriggerModeStream << "> Trigger mode: ";

    if (aPoisson) {
        lTriggerModeStream << "poisson";
    } else {
        lTriggerModeStream << "periodic";
    }
    PDT_LOG(kNotice) << lTriggerModeStream.str();
    getNode<FLCmdGeneratorNode>("master.scmd_gen").enableFakeTrigger(aChan, lFTConfig.mDiv, lFTConfig.mPS, aPoisson);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::disableFakeTrigger(uint32_t aChan) const {
    getNode<FLCmdGeneratorNode>("master.scmd_gen").disableFakeTrigger(aChan);
}
//------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::enableSpillInterface() const {
     getNode<SpillInterfaceNode>("master.spill").enable();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::enableFakeSpills(uint32_t aCycLen, uint32_t aSpillLen) const {
    getNode<SpillInterfaceNode>("master.spill").enableFakeSpills(aCycLen, aSpillLen);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
PDIMasterNode::readInSpill() const {
    return getNode<SpillInterfaceNode>("master.spill").readInSpill();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::resetExternalTriggersEndpoint() const {
    getNode<TriggerReceiverNode>("trig").reset();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::enableExternalTriggers() const {
    getNode<TriggerReceiverNode>("trig").enableTriggers();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::disableExternalTriggers() const {
    getNode<TriggerReceiverNode>("trig").disableTriggers();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
const PartitionNode&
PDIMasterNode::getPartitionNode(uint32_t aPartID) const {
    const std::string nodeName = "master.partition" + std::to_string(aPartID);
    return getNode<PartitionNode>(nodeName);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::syncTimestamp() const {

    const uint64_t lOldTimestamp = readTimestamp();
    PDT_LOG(kNotice) << "Old timestamp: 0x" << std::hex << lOldTimestamp << ", " << formatTimestamp(lOldTimestamp);
    
    const uint64_t lNow = getSecondsSinceEpoch() * kSPSClockInHz;
    setTimestamp(lNow);

    const uint64_t lNewTimestamp = readTimestamp();
    PDT_LOG(kNotice) << "New timestamp: 0x" << std::hex << lNewTimestamp << ", " << formatTimestamp(lNewTimestamp);
    }
//-----------------------------------------------------------------------------

} // namespace pdt