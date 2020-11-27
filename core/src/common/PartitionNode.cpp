#include "pdt/PartitionNode.hpp"

#include <chrono>
#include <thread>

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(PartitionNode);

// Static data member initialization
const uint32_t PartitionNode::kWordsPerEvent = 6;


//-----------------------------------------------------------------------------
PartitionNode::PartitionNode(const uhal::Node& aNode) : TimingNode(aNode) {

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
// void
// PartitionNode::writeTriggerMask( uint32_t aMask) const {
//     getNode("csr.ctrl.trig_mask").write(aMask);
//     getClient().dispatch();
// }
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PartitionNode::configure( uint32_t aTrigMask, bool aEnableSpillGate, bool aRateCtrl) const {
    getNode("csr.ctrl.rate_ctrl_en").write(aRateCtrl);
    getNode("csr.ctrl.trig_mask").write(aTrigMask);
    getNode("csr.ctrl.spill_gate_en").write(aEnableSpillGate);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PartitionNode::configureRateCtrl( bool aRateCtrl) const {
    getNode("csr.ctrl.rate_ctrl_en").write(aRateCtrl);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PartitionNode::enableTriggers( bool aEnable ) const {
    // Disable the buffer
    getNode("csr.ctrl.trig_en").write(aEnable);
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
bool
PartitionNode::readROBWarningOverflow() const {
    uhal::ValWord<uint32_t> lWord = getNode("csr.stat.buf_warn").read();
    getClient().dispatch();

    return lWord.value();

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
PartitionNode::readROBError() const {
    uhal::ValWord<uint32_t> lWord = getNode("csr.stat.buf_err").read();
    getClient().dispatch();

    return lWord.value();

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
        if ( (end - start) > std::chrono::milliseconds(aTimeout) ) {
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
        if ( (end - start) > std::chrono::milliseconds(aTimeout) ) {
            std::ostringstream lMsg;
            lMsg << "Failed to stop after " << aTimeout << " milliseconds";
            throw RunRequestTimeoutExpired(lMsg.str());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
PartitionCounts
PartitionNode::readCommandCounts() const {

    const uhal::Node& lAccCtrs = getNode("actrs");
    const uhal::Node& lRejCtrs = getNode("rctrs");
    
    uhal::ValVector<uint32_t> lAccepted = lAccCtrs.readBlock(lAccCtrs.getSize());
    uhal::ValVector<uint32_t> lRejected = lRejCtrs.readBlock(lRejCtrs.getSize());
    getClient().dispatch();

    return {lAccepted.value(), lRejected.value()};

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
PartitionNode::getStatus(bool aPrint) const {
    std::stringstream lStatus;

    auto lControls = readSubNodes(getNode("csr.ctrl"), false);
    auto lState = readSubNodes(getNode("csr.stat"), false);

    auto lEventCtr = getNode("evtctr").read();
    auto lBufCount = getNode("buf.count").read();

    auto lAccCounters = getNode("actrs").readBlock(getNode("actrs").getSize());
    auto lRejCounters = getNode("rctrs").readBlock(getNode("actrs").getSize());
    
    getClient().dispatch();

    std::string lPartID = getId();
    std::string lPartNum = lPartID.substr(lPartID.find("partition") + 9) ;
    
    lStatus << "=> Partition " << lPartNum << std::endl;
    lStatus << std::endl;

    lStatus << formatRegTable(lControls, "Controls") << std::endl;
    lStatus << formatRegTable(lState, "State") << std::endl;

    lStatus << "Event Counter: " << lEventCtr.value() << std::endl;
    std::string lBufferStatusString = !lState.find("buf_err")->second.value() ? "OK" : "Error";
    lStatus << "Buffer status: " << lBufferStatusString << std::endl;
    lStatus << "Buffer occupancy: " << lBufCount.value() << std::endl;

    lStatus << std::endl;

    std::vector<uhal::ValVector<uint32_t>> lCountersContainer = {lAccCounters, lRejCounters};

    lStatus << formatCountersTable(lCountersContainer, {"Accept counters", "Reject counters"});

    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------

} // namespace pdt
