#include "pdt/PartitionNode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(PartitionNode);

//-----------------------------------------------------------------------------
PartitionNode::PartitionNode(const uhal::Node& aNode) : uhal::Node(aNode) {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
PartitionNode::~PartitionNode() {

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PartitionNode::enableNow(bool aEnable) const {
    getNode("csr.ctrl.part_en").write(1);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PartitionNode::reset() const {
    // Disable partition 0
    getNode("csr.ctrl.part_en").write(0);
    // Disable buffer in partition 0
    getNode("csr.ctrl.buf_en").write(0);
    // Reset event counter
    getNode("csr.ctrl.evtctr_rst").write(1);
    // Reset trigger counter
    getNode("csr.ctrl.trig_ctr_rst").write(1);
    // Release event counter
    getNode("csr.ctrl.evtctr_rst").write(0);
    // Release trigger counter
    getNode("csr.ctrl.trig_ctr_rst").write(0);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::start() const {
    // Disable the buffer
    getNode("csr.ctrl.buf_en").write(0);
    // TODO: in configuration?
    // Set command mask in partition 0
    // getNode('csr.ctrl.cmd_mask').write(0x0f) 
    getClient().dispatch();
    
    // TODO (v4)
    // Set the run bit and wait for it to be acknowledged

    // Re-enable the buffer (flushes it)
    getNode("csr.ctrl.buf_en").write(1);
    // Open to triggers
    getNode("csr.ctrl.trig_en").write(1);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PartitionNode::stop() const {
    // Disable the readout buffer
    getNode("csr.ctrl.buf_en").write(0);
    // Disable triggers
    getNode("csr.ctrl.trig_en").write(0);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

} // namespace pdt