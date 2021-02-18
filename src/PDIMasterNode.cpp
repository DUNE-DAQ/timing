#include "pdt/PDIMasterNode.hpp"

namespace dunedaq {
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
PDIMasterNode::get_status(bool aPrint) const {
    std::stringstream lStatus;
    auto lTStamp = getNode<TimestampGeneratorNode>("master.tstamp").read_raw_timestamp();
    lStatus << "Timestamp: 0x" << std::hex << tstamp2int(lTStamp) << " -> " << format_timestamp(lTStamp) << std::endl << std::endl;
    lStatus << getNode<FLCmdGeneratorNode>("master.scmd_gen").get_cmd_counters_table();
    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::switch_endpoint_sfp(uint32_t aAddr, bool aOn) const {
    auto vlCmdNode = getNode<VLCmdGeneratorNode>("master.acmd");

    // Switch off endpoint SFP tx
    vlCmdNode.switch_endpoint_sfp(aAddr, aOn);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::enable_upstream_endpoint() const {
    auto lGlobal = getNode<GlobalNode>("master.global");
    lGlobal.enable_upstream_endpoint();
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
        vlCmdNode.switch_endpoint_sfp(0x0, false);
    
        // Turn on the current target
        vlCmdNode.switch_endpoint_sfp(aAddr, true);
    
        millisleep(100);
    }

    lGlobal.enable_upstream_endpoint();
        
    uint32_t lEndpointRTT = lEcho.send_echo_and_measure_delay();
    
    if (aControlSFP) vlCmdNode.switch_endpoint_sfp(aAddr, false);

    return lEndpointRTT;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::apply_endpoint_delay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT, bool aControlSFP) const {
    
    auto vlCmdNode = getNode<VLCmdGeneratorNode>("master.acmd");
    auto lGlobal = getNode<GlobalNode>("master.global");
    auto lEcho = getNode<EchoMonitorNode>("master.echo");

    if (aMeasureRTT) {
        
        if (aControlSFP) {
            // Switch off all TX SFPs
            vlCmdNode.switch_endpoint_sfp(0x0, false);
            
            // Turn on the current target
            vlCmdNode.switch_endpoint_sfp(aAddr, true);
        
            millisleep(100);
        }

        lGlobal.enable_upstream_endpoint();
        
        uint64_t lEndpointRTT = lEcho.send_echo_and_measure_delay();
        ERS_LOG("Pre delay adjustment RTT:  " << format_reg_value(lEndpointRTT)); 
    }
    
    vlCmdNode.apply_endpoint_delay(aAddr, aCDel, aFDel, aPDel);

    millisleep(100);

    if (aMeasureRTT) {
        lGlobal.enable_upstream_endpoint();

        uint64_t lEndpointRTT = lEcho.send_echo_and_measure_delay();
        ERS_LOG("Post delay adjustment RTT: " << format_reg_value(lEndpointRTT));

        if (aControlSFP) vlCmdNode.switch_endpoint_sfp(aAddr, false);
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::send_fl_cmd(uint32_t aCmd, uint32_t aChan, uint32_t aNumber) const {
    for (uint32_t i=0; i < aNumber; i++) {
        getNode<FLCmdGeneratorNode>("master.scmd_gen").send_fl_cmd(aCmd, aChan, getNode<TimestampGeneratorNode>("master.tstamp"));
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::enable_fake_trigger(uint32_t aChan, double aRate, bool aPoisson) const {

    // Configures the internal command generator to produce triggers at a defined frequency.
    // Rate =  (50MHz / 2^(d+8)) / p where n in [0,15] and p in [1,256]

    // DIVIDER (int): Frequency divider.
    

    // The division from 50MHz to the desired rate is done in three steps:
    // a) A pre-division by 256
    // b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
    // c) 1-in-n prescaling set by n = rate_div_p
    
    FakeTriggerConfig lFTConfig(aRate);

    lFTConfig.print();
    std::stringstream trig_stream;
    trig_stream << "> Trigger rate for FakeTrig" << aChan << " (" << std::showbase << std::hex << 0x8+aChan << ") set to " << std::setprecision(3) << std::scientific << lFTConfig.mActRate << " Hz";
    ERS_INFO(trig_stream.str());

    std::stringstream lTriggerModeStream;
    lTriggerModeStream << "> Trigger mode: ";

    if (aPoisson) {
        lTriggerModeStream << "poisson";
    } else {
        lTriggerModeStream << "periodic";
    }
    ERS_INFO(lTriggerModeStream.str());
    getNode<FLCmdGeneratorNode>("master.scmd_gen").enable_fake_trigger(aChan, lFTConfig.mDiv, lFTConfig.mPS, aPoisson);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::disable_fake_trigger(uint32_t aChan) const {
    getNode<FLCmdGeneratorNode>("master.scmd_gen").disable_fake_trigger(aChan);
}
//------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::enable_spill_interface() const {
     getNode<SpillInterfaceNode>("master.spill").enable();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::enable_fake_spills(uint32_t aCycLen, uint32_t aSpillLen) const {
    getNode<SpillInterfaceNode>("master.spill").enable_fake_spills(aCycLen, aSpillLen);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
PDIMasterNode::read_in_spill() const {
    return getNode<SpillInterfaceNode>("master.spill").read_in_spill();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::reset_external_triggers_endpoint() const {
    getNode<TriggerReceiverNode>("trig").reset();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::enable_external_triggers() const {
    getNode<TriggerReceiverNode>("trig").enable_triggers();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::disable_external_triggers() const {
    getNode<TriggerReceiverNode>("trig").disable_triggers();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
const PartitionNode&
PDIMasterNode::get_partition_node(uint32_t aPartID) const {
    const std::string nodeName = "master.partition" + std::to_string(aPartID);
    return getNode<PartitionNode>(nodeName);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIMasterNode::sync_timestamp() const {

    const uint64_t lOldTimestamp = read_timestamp();
    ERS_LOG("Old timestamp: " << format_reg_value(lOldTimestamp) << ", " << format_timestamp(lOldTimestamp));
    
    const uint64_t lNow = get_seconds_since_epoch() * kSPSClockInHz;
    set_timestamp(lNow);

    const uint64_t lNewTimestamp = read_timestamp();
    ERS_LOG("New timestamp: " << format_reg_value(lNewTimestamp) << ", " << format_timestamp(lNewTimestamp));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::get_info(timingmon::TimingPDIMasterMonitorData& mon_data) const {
    auto lTStamp = getNode<TimestampGeneratorNode>("master.tstamp").read_raw_timestamp();
    mon_data.timestamp = tstamp2int(lTStamp);

    auto spill_interface_enabled = getNode("master.spill.csr.ctrl.en").read();
    auto trig_interface_enabled = getNode("trig.csr.ctrl.ep_en").read();
    getClient().dispatch();

    mon_data.spill_interface_enabled = spill_interface_enabled.value();
    mon_data.trig_interface_enabled = trig_interface_enabled.value();

    getNode<FLCmdGeneratorNode>("master.scmd_gen").get_info(mon_data.command_counters);

    for (uint i=0; i < 4; ++i) {
        timingmon::TimingPartitionMonitorData partition_data;
        get_partition_node(i).get_info(partition_data);
        mon_data.partitions_data.push_back(partition_data);
    }

}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq