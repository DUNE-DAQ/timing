#include "pdt/PartitionNode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(PartitionNode);

// Static data member initialization
const uint32_t PartitionNode::kWordsPerEvent = 6;


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
PartitionNode::enable(bool aEnable, bool aDispatch) const {
    getNode("csr.ctrl.part_en").write(aEnable);
    
    if ( aDispatch )
        getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PartitionNode::setCommandMask( uint32_t aMask) const {
    getNode("csr.ctrl.cmd_mask").write(aMask);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
PartitionNode::readCommandMask() const {
    uhal::ValWord<uint32_t> lMask = getNode("csr.ctrl.cmd_mask").read();
    getClient().dispatch();

    return lMask;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
PartitionNode::readBufferWordCount() const {
    uhal::ValWord<uint32_t> lWords = getNode("buf.count").read();
    getClient().dispatch();

    return lWords;

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
PartitionNode::numEventsInBuffer() const {
    return readBufferWordCount() / kWordsPerEvent;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint32_t>
PartitionNode::readEvents( size_t aNumEvents ) const {

    uint32_t lEventsInBuffer = numEventsInBuffer();

    uint32_t lEventsToRead = ( aNumEvents == 0 ? lEventsInBuffer : aNumEvents);

    if (lEventsInBuffer < lEventsToRead ) {
        std::ostringstream lMsg;
        lMsg << "Failed to read events from partition buffer. Requested: " << aNumEvents << " Available: " << lEventsInBuffer;
        throw EventReadError(lMsg.str());
    }  

    uhal::ValVector<uint32_t> lRawEvents = getNode("buf.data").readBlock(lEventsToRead * kWordsPerEvent);
    getClient().dispatch();

    return lRawEvents.value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PartitionNode::reset() const {
    // Disable partition
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
    // getNode("csr.ctrl.cmd_mask").write(0x0f) 
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