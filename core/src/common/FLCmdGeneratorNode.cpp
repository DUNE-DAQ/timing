#include "pdt/FLCmdGeneratorNode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(FLCmdGeneratorNode);

//-----------------------------------------------------------------------------
FLCmdGeneratorNode::FLCmdGeneratorNode(const uhal::Node& aNode) : TimingNode(aNode) {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
FLCmdGeneratorNode::~FLCmdGeneratorNode() {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
FLCmdGeneratorNode::getStatus(bool aPrint) const {
    std::stringstream lStatus;
    auto subnodes = readSubNodes(getNode("csr.stat"));
    lStatus << formatRegTable(subnodes, "FL Cmd gen state");
    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::sendFLCmd(uint32_t aCmd, uint32_t aChan, const TimestampGeneratorNode& aTSGen) const {
    getNode("sel").write(aChan);

    resetSubNodes(getNode("chan_ctrl"));

    getNode("chan_ctrl.type").write(aCmd);
    getNode("chan_ctrl.force").write(0x1);
    auto lTStamp = aTSGen.readRawTimestamp(false);
    
    getClient().dispatch();
    
    getNode("chan_ctrl.force").write(0x0);
    getClient().dispatch();
    PDT_LOG(kNotice) << "Command sent " << kCommandMap.at(aCmd) << "(" << aCmd << ") from generator " << aChan << " @time 0x" << tstamp2int(lTStamp) << " " << formatTimestamp(lTStamp);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::enableFakeTrigger(uint32_t aChan, uint32_t aDiv, uint32_t aPS, bool aPoisson) const {
    uint32_t trigCmd = 0x8 + aChan;

    getNode("sel").write(aChan);

    getNode("chan_ctrl.type").write(trigCmd);
    getNode("chan_ctrl.rate_div_d").write(aDiv);
    getNode("chan_ctrl.rate_div_p").write(aPS);
    getNode("chan_ctrl.patt").write(aPoisson);
    getNode("chan_ctrl.en").write(1); // Start the command stream
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::disableFakeTrigger(uint32_t aChan) const {
    //Clear the internal trigger generator.
    getNode("sel").write(aChan);
    resetSubNodes(getNode("chan_ctrl"));
    PDT_LOG(kInfo) << "Fake trigger generator " << aChan << " configuration cleared";
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::string
FLCmdGeneratorNode::getCmdCountersTable(bool aPrint) const {
    std::stringstream lCountersTable;
    auto lAccCounters = getNode("actrs").readBlock(getNode("actrs").getSize());
    auto lRejCounters = getNode("rctrs").readBlock(getNode("actrs").getSize());
    getClient().dispatch();

    std::vector<uhal::ValVector<uint32_t>> lCountersContainer = {lAccCounters, lRejCounters};

    lCountersTable << formatCountersTable(lCountersContainer, {"Accept counters", "Reject counters"}, "Cmd gen counters", {"0x0","0x1","0x2","0x3","0x4"}, "Chan");

    if (aPrint) std::cout << lCountersTable.str();
    return lCountersTable.str();
}
//-----------------------------------------------------------------------------
} // namespace pdt