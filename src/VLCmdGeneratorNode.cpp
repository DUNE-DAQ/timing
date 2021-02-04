#include "pdt/VLCmdGeneratorNode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(VLCmdGeneratorNode)

//-----------------------------------------------------------------------------
VLCmdGeneratorNode::VLCmdGeneratorNode(const uhal::Node& aNode) : TimingNode(aNode) {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
VLCmdGeneratorNode::~VLCmdGeneratorNode() {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
VLCmdGeneratorNode::getStatus(bool aPrint) const {
    std::stringstream lStatus;
    auto subnodes = readSubNodes(getNode("csr.stat"));
    lStatus << formatRegTable(subnodes, "VL Cmd gen state");

    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
VLCmdGeneratorNode::switchEndpointSFP(uint32_t aAddr, bool aEnable) const {
    resetSubNodes(getNode("csr.ctrl"));
    getNode("csr.ctrl.addr").write(aAddr);
    getNode("csr.ctrl.tx_en").write(aEnable);
    getNode("csr.ctrl.go").write(0x1);
    getNode("csr.ctrl.go").write(0x0);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
VLCmdGeneratorNode::applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel) const {
    resetSubNodes(getNode("csr.ctrl"), false);
    getNode("csr.ctrl.tx_en").write(0x1);
    getNode("csr.ctrl.addr").write(aAddr);
    getNode("csr.ctrl.cdel").write(aCDel);
    getNode("csr.ctrl.fdel").write(aFDel);
    getNode("csr.ctrl.pdel").write(aPDel);
    getNode("csr.ctrl.update").write(0x1);
    getNode("csr.ctrl.go").write(0x1);
    getNode("csr.ctrl.go").write(0x0);
    getClient().dispatch();

    ERS_LOG("Coarse delay " << formatRegValue(aCDel) << " applied");
    ERS_LOG("Fine delay   " << formatRegValue(aFDel) << " applied");
    ERS_LOG("Phase delay  " << formatRegValue(aPDel) << " applied");
}
//-----------------------------------------------------------------------------


} // namespace pdt