#include "timing/VLCmdGeneratorNode.hpp"

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(VLCmdGeneratorNode)

//-----------------------------------------------------------------------------
VLCmdGeneratorNode::VLCmdGeneratorNode(const uhal::Node& node) : TimingNode(node) {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
VLCmdGeneratorNode::~VLCmdGeneratorNode() {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
VLCmdGeneratorNode::get_status(bool print_out) const {
    std::stringstream lStatus;
    auto subnodes = read_sub_nodes(getNode("csr.stat"));
    lStatus << format_reg_table(subnodes, "VL Cmd gen state");

    if (print_out) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
VLCmdGeneratorNode::switch_endpoint_sfp(uint32_t address, bool enable) const {
    reset_sub_nodes(getNode("csr.ctrl"));
    getNode("csr.ctrl.addr").write(address);
    getNode("csr.ctrl.tx_en").write(enable);
    getNode("csr.ctrl.go").write(0x1);
    getNode("csr.ctrl.go").write(0x0);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
VLCmdGeneratorNode::apply_endpoint_delay(uint32_t address, uint32_t coarse_delay, uint32_t fine_delay, uint32_t phase_delay) const {
    reset_sub_nodes(getNode("csr.ctrl"), false);
    getNode("csr.ctrl.tx_en").write(0x1);
    getNode("csr.ctrl.addr").write(address);
    getNode("csr.ctrl.cdel").write(coarse_delay);
    getNode("csr.ctrl.fdel").write(fine_delay);
    getNode("csr.ctrl.pdel").write(phase_delay);
    getNode("csr.ctrl.update").write(0x1);
    getNode("csr.ctrl.go").write(0x1);
    getNode("csr.ctrl.go").write(0x0);
    getClient().dispatch();

    TLOG() << "Coarse delay " << format_reg_value(coarse_delay) << " applied";
    TLOG() << "Fine delay   " << format_reg_value(fine_delay) << " applied";
    TLOG() << "Phase delay  " << format_reg_value(phase_delay) << " applied";
}
//-----------------------------------------------------------------------------


} // namespace timing
} // namespace dunedaq