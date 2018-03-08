#include "pdt/PartitionNode.hpp"

#include <chrono>
#include <thread>

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
PartitionNode::writeTriggerMask( uint32_t aMask) const {
    getNode("csr.ctrl.trig_mask").write(aMask);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PartitionNode::enableTriggers( bool aEnable ) const {
    // Disable the buffer
    getNode("csr.ctrl.buf_en").write(aEnable);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
PartitionNode::readTriggerMask() const {
    uhal::ValWord<uint32_t> lMask = getNode("csr.ctrl.trig_mask").read();
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
    // Reset trigger counter
    getNode("csr.ctrl.trig_ctr_rst").write(1);
    // Release trigger counter
    getNode("csr.ctrl.trig_ctr_rst").write(0);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::start( uint32_t aTimeout /*milliseconds*/ ) const {

    // Disable triggers (just in case)
    getNode("csr.ctrl.trig_en").write(0);
    // Disable the buffer
    getNode("csr.ctrl.buf_en").write(0);
    getClient().dispatch();
    // Re-enable the buffer (flushes it)
    getNode("csr.ctrl.buf_en").write(1);
    getClient().dispatch();
    
    // Set the run bit and wait for it to be acknowledged
    getNode("csr.ctrl.run_req").write(1);
    getClient().dispatch();


    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    while(true) {
        auto lInRun = getNode("csr.stat.in_run").read();
        getClient().dispatch();

        if ( lInRun ) break;

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        if ( (end - start).count() > aTimeout or true) {
            std::ostringstream lMsg;
            lMsg << "Failed to start after " << aTimeout << " milliseconds";
            throw RunRequestTimeoutExpired(lMsg.str());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PartitionNode::stop( uint32_t aTimeout /*milliseconds*/ ) const {
    getNode("csr.ctrl.run_req").write(0);
    getClient().dispatch();

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    while(true) {

        auto lInRun = getNode("csr.stat.in_run").read();
        getClient().dispatch();

        if ( !lInRun ) break;

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        if ( (end - start).count() > aTimeout or true) {
            std::ostringstream lMsg;
            lMsg << "Failed to stop after " << aTimeout << " milliseconds";
            throw RunRequestTimeoutExpired(lMsg.str());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
//-----------------------------------------------------------------------------

} // namespace pdt