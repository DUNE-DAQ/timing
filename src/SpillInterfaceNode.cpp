#include "pdt/SpillInterfaceNode.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(SpillInterfaceNode)

//-----------------------------------------------------------------------------
SpillInterfaceNode::SpillInterfaceNode(const uhal::Node& aNode) : TimingNode(aNode) {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SpillInterfaceNode::~SpillInterfaceNode() {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SpillInterfaceNode::get_status(bool aPrint) const {
    std::stringstream lStatus;
    auto subnodes = read_sub_nodes(getNode("csr.stat"));
    lStatus << format_reg_table(subnodes, "Spill interface state");
    
    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SpillInterfaceNode::enable() const {
    getNode("csr.ctrl.src").write(0);
    getNode("csr.ctrl.en").write(1);
    getClient().dispatch();
    ERS_INFO("Spill interface enabled");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SpillInterfaceNode::disable() const {
    getNode("csr.ctrl.en").write(0);
    getClient().dispatch();
    ERS_INFO("Spill interface disabled");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SpillInterfaceNode::enable_fake_spills(uint32_t aCycLen, uint32_t aSpillLen) const {


    // Enables the internal spill generator.
    // Configures the internal command generator to produce spills at a defined frequency and length
    
    // Rate = 50 Mhz / 2**( 12 + divider ) for divider between 0 and 15
    // cyc_len and spill_len are in units of 1 / (50MHz / 2^24) = 0.34s

    getNode("csr.ctrl.fake_cyc_len").write(aCycLen);
    getNode("csr.ctrl.fake_spill_len").write(aSpillLen);
    getNode("csr.ctrl.src").write(1);
    getNode("csr.ctrl.en").write(1);
    getClient().dispatch();
    ERS_INFO("Fake spills enabled");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
SpillInterfaceNode::read_in_spill() const {
    auto lInSpill = getNode("csr.stat.in_spill").read();
    getClient().dispatch();
    return lInSpill.value();
}
//------------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq