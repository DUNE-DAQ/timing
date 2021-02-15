#include "pdt/TriggerReceiverNode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(TriggerReceiverNode)

//-----------------------------------------------------------------------------
TriggerReceiverNode::TriggerReceiverNode(const uhal::Node& aNode) : TimingNode(aNode) {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
TriggerReceiverNode::~TriggerReceiverNode() {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
TriggerReceiverNode::get_status(bool aPrint) const {
    std::stringstream lStatus;

    auto lState = read_sub_nodes(getNode("csr.stat"), false);
    auto lControls = read_sub_nodes(getNode("csr.ctrl"), false);
    auto lCounters = getNode("ctrs").readBlock(0x10);
    getClient().dispatch();

    lStatus << formatRegTable(lState, "Trigger rx state");
    lStatus << formatRegTable(lControls, "Trigger rx controls");

    std::vector<uhal::ValVector<uint32_t>> lCountersContainer = {lCounters};
    lStatus << formatCountersTable(lCountersContainer, {"Counters"}, "Trig rx counters");

    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TriggerReceiverNode::enable() const {
    getNode("csr.ctrl.ep_en").write(0x1);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TriggerReceiverNode::disable() const {
    getNode("csr.ctrl.ep_en").write(0x0);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TriggerReceiverNode::reset() const {
    getNode("csr.ctrl.ep_en").write(0x0);
    getNode("csr.ctrl.ep_en").write(0x1);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TriggerReceiverNode::enableTriggers() const {
    getNode("csr.ctrl.ext_trig_en").write(0x1);
    getClient().dispatch();
}
//------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TriggerReceiverNode::disableTriggers() const {
    getNode("csr.ctrl.ext_trig_en").write(0x0);
    getClient().dispatch();
}
//------------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq