#include "pdt/FLCmdGeneratorNode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(FLCmdGeneratorNode)

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
FLCmdGeneratorNode::get_status(bool aPrint) const {
    std::stringstream lStatus;
    auto subnodes = read_sub_nodes(getNode("csr.stat"));
    lStatus << format_reg_table(subnodes, "FL Cmd gen state");
    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::send_fl_cmd(uint32_t aCmd, uint32_t aChan, const TimestampGeneratorNode& aTSGen) const {
    getNode("sel").write(aChan);

    reset_sub_nodes(getNode("chan_ctrl"));

    getNode("chan_ctrl.type").write(aCmd);
    getNode("chan_ctrl.force").write(0x1);
    auto lTStamp = aTSGen.read_raw_timestamp(false);
    
    getClient().dispatch();
    
    getNode("chan_ctrl.force").write(0x0);
    getClient().dispatch();
    ERS_LOG("Command sent " << kCommandMap.at(aCmd) << "(" << format_reg_value(aCmd) << ") from generator " << format_reg_value(aChan) << " @time 0x" << tstamp2int(lTStamp) << " " << format_timestamp(lTStamp));
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::enable_fake_trigger(uint32_t aChan, uint32_t aDiv, uint32_t aPS, bool aPoisson) const {
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
FLCmdGeneratorNode::disable_fake_trigger(uint32_t aChan) const {
    //Clear the internal trigger generator.
    getNode("sel").write(aChan);
    reset_sub_nodes(getNode("chan_ctrl"));
    ERS_LOG("Fake trigger generator " << format_reg_value(aChan) << " configuration cleared");
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::string
FLCmdGeneratorNode::get_cmd_counters_table(bool aPrint) const {
    std::stringstream lCountersTable;
    auto lAccCounters = getNode("actrs").readBlock(getNode("actrs").getSize());
    auto lRejCounters = getNode("rctrs").readBlock(getNode("actrs").getSize());
    getClient().dispatch();

    std::vector<uhal::ValVector<uint32_t>> lCountersContainer = {lAccCounters, lRejCounters};

    lCountersTable << format_counters_table(lCountersContainer, {"Accept counters", "Reject counters"}, "Cmd gen counters", {"0x0","0x1","0x2","0x3","0x4"}, "Chan");

    if (aPrint) std::cout << lCountersTable.str();
    return lCountersTable.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::get_info(timingmon::TimingFLCmdCountersVector& mon_data) const {
    auto accepted_counters = getNode("actrs").readBlock(getNode("actrs").getSize());
    auto rejected_counters = getNode("rctrs").readBlock(getNode("actrs").getSize());
    getClient().dispatch();

    uint counters_number = getNode("actrs").getSize();

    for (uint i=0; i < counters_number; ++i) {
        timingmon::TimingFLCmdCounters fl_cmd_counters;
        fl_cmd_counters.accepted = accepted_counters.at(i);
        fl_cmd_counters.rejected = rejected_counters.at(i);
        mon_data.push_back(fl_cmd_counters);   
    }
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq