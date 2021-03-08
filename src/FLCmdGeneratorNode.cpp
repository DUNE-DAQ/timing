#include "pdt/FLCmdGeneratorNode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(FLCmdGeneratorNode)

//-----------------------------------------------------------------------------
FLCmdGeneratorNode::FLCmdGeneratorNode(const uhal::Node& node) : TimingNode(node) {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
FLCmdGeneratorNode::~FLCmdGeneratorNode() {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
FLCmdGeneratorNode::get_status(bool print_out) const {
    std::stringstream lStatus;
    auto subnodes = read_sub_nodes(getNode("csr.stat"));
    lStatus << format_reg_table(subnodes, "FL Cmd gen state");
    if (print_out) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::send_fl_cmd(uint32_t command, uint32_t channel, const TimestampGeneratorNode& timestamp_gen_node) const {
    getNode("sel").write(channel);

    reset_sub_nodes(getNode("chan_ctrl"));

    getNode("chan_ctrl.type").write(command);
    getNode("chan_ctrl.force").write(0x1);
    auto lTStamp = timestamp_gen_node.read_raw_timestamp(false);
    
    getClient().dispatch();
    
    getNode("chan_ctrl.force").write(0x0);
    getClient().dispatch();
    TLOG() << "Command sent " << g_command_map.at(command) << "(" << format_reg_value(command) << ") from generator " << format_reg_value(channel) << " @time 0x" << tstamp2int(lTStamp) << " " << format_timestamp(lTStamp);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::enable_fake_trigger(uint32_t channel, uint32_t divisor, uint32_t prescale, bool poisson) const {
    uint32_t trigCmd = 0x8 + channel;

    getNode("sel").write(channel);

    getNode("chan_ctrl.type").write(trigCmd);
    getNode("chan_ctrl.rate_div_d").write(divisor);
    getNode("chan_ctrl.rate_div_p").write(prescale);
    getNode("chan_ctrl.patt").write(poisson);
    getNode("chan_ctrl.en").write(1); // Start the command stream
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::disable_fake_trigger(uint32_t channel) const {
    //Clear the internal trigger generator.
    getNode("sel").write(channel);
    reset_sub_nodes(getNode("chan_ctrl"));
    TLOG() << "Fake trigger generator " << format_reg_value(channel) << " configuration cleared";
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::string
FLCmdGeneratorNode::get_cmd_counters_table(bool print_out) const {
    std::stringstream lCountersTable;
    auto lAccCounters = getNode("actrs").readBlock(getNode("actrs").getSize());
    auto lRejCounters = getNode("rctrs").readBlock(getNode("actrs").getSize());
    getClient().dispatch();

    std::vector<uhal::ValVector<uint32_t>> lCountersContainer = {lAccCounters, lRejCounters};

    lCountersTable << format_counters_table(lCountersContainer, {"Accept counters", "Reject counters"}, "Cmd gen counters", {"0x0","0x1","0x2","0x3","0x4"}, "Chan");

    if (print_out) std::cout << lCountersTable.str();
    return lCountersTable.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::get_info(timingfirmwareinfo::TimingFLCmdCountersVector& mon_data) const {
    auto accepted_counters = getNode("actrs").readBlock(getNode("actrs").getSize());
    auto rejected_counters = getNode("rctrs").readBlock(getNode("actrs").getSize());
    getClient().dispatch();

    //uint counters_number = getNode("actrs").getSize();

    // 5 cmd channels
    uint counters_number = 5;

    for (uint i=0; i < counters_number; ++i) {
        timingfirmwareinfo::TimingFLCmdCounters fl_cmd_counters;
        fl_cmd_counters.accepted = accepted_counters.at(i);
        fl_cmd_counters.rejected = rejected_counters.at(i);
        mon_data.push_back(fl_cmd_counters);   
    }
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq