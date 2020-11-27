#include "pdt/EndpointNode.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(EndpointNode);

//-----------------------------------------------------------------------------
EndpointNode::EndpointNode(const uhal::Node& aNode) : TimingNode(aNode) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
EndpointNode::~EndpointNode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
EndpointNode::enable(uint32_t partition, uint32_t address) const {
	getNode("csr.ctrl.tgrp").write(partition);

	if (address) {
		getNode("csr.ctrl.int_addr").write(0x1);
		getNode("csr.ctrl.addr").write(address);
	} else {
		getNode("csr.ctrl.int_addr").write(0x0);
	}

	getNode("csr.ctrl.ctr_rst").write(0x1);
	getNode("csr.ctrl.ctr_rst").write(0x0);
	getNode("csr.ctrl.ep_en").write(0x1);
	getNode("csr.ctrl.buf_en").write(0x1);
	getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
EndpointNode::disable() const {
	getNode("csr.ctrl.ep_en").write(0x0);
    getNode("csr.ctrl.buf_en").write(0x0);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
EndpointNode::reset(uint32_t partition, uint32_t address) const {

	getNode("csr.ctrl.ep_en").write(0x0);
	getNode("csr.ctrl.buf_en").write(0x0);
	
	enable(partition, address);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
EndpointNode::getStatus(bool aPrint) const {
	
	std::stringstream lStatus;

	std::vector<std::pair<std::string, std::string> > lEPSummary;

	auto lEPTimestamp = getNode("tstamp").readBlock(2);
	auto lEPEventCounter = getNode("evtctr").read();
	auto lEPBufferCount = getNode("buf.count").read();
	auto lEPControl = readSubNodes(getNode("csr.ctrl"), false);
	auto lEPState = readSubNodes(getNode("csr.stat"), false);
	auto lEPCounters = getNode("ctrs").readBlock(kCommandNumber);
	getClient().dispatch();

	lEPSummary.push_back(std::make_pair("State", kEndpointStateMap.at(lEPState.find("ep_stat")->second.value())));
	lEPSummary.push_back(std::make_pair("Partition", std::to_string(lEPControl.find("tgrp")->second.value())));
	lEPSummary.push_back(std::make_pair("Address", std::to_string(lEPControl.find("addr")->second.value())));
	lEPSummary.push_back(std::make_pair("Timestamp", formatTimestamp(lEPTimestamp)));
	lEPSummary.push_back(std::make_pair("Timestamp (hex)", formatRegValue(tstamp2int(lEPTimestamp))));
	lEPSummary.push_back(std::make_pair("EventCounter", std::to_string(lEPEventCounter.value())));
	std::string lBufferStatusString = !lEPState.find("buf_err")->second.value() ? "OK" : "Error";
	lEPSummary.push_back(std::make_pair("Buffer status", lBufferStatusString));
	lEPSummary.push_back(std::make_pair("Buffer occupancy", std::to_string(lEPBufferCount.value())));

	std::vector<std::pair<std::string, std::string> > lEPCommandCounters;

	for (uint32_t i=0; i < kCommandNumber; ++i) {
		lEPCommandCounters.push_back(std::make_pair(kCommandMap.at(i), std::to_string(lEPCounters[i])));	
	}

	lStatus << formatRegTable(lEPSummary, "Endpoint summary", {"", ""}) << std::endl;
	lStatus << formatRegTable(lEPState, "Endpoint state") << std::endl;
	lStatus << formatRegTable(lEPCommandCounters, "Endpoint counters", {"Command", "Counter"}); 

	if (aPrint) std::cout << lStatus.str();
    return lStatus.str();       
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint64_t
EndpointNode::readTimestamp() const {
	auto lTimestamp = getNode("tstamp").readBlock(2);
	getClient().dispatch();
    return tstamp2int(lTimestamp);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
EndpointNode::readBufferCount() const {
	auto lBufCount = getNode("buf.count").read();
	getClient().dispatch();
	return lBufCount.value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uhal::ValVector< uint32_t >
EndpointNode::readDataBuffer(bool aReadall) const {
	
	auto lBufCount = getNode("buf.count").read();
	getClient().dispatch();

	PDT_LOG(kNotice) << "Words available in readout buffer:      " << formatRegValue(lBufCount.value());

	uint32_t lEventsToRead = lBufCount.value() / kEventSize;
	
	PDT_LOG(kNotice) << "Events available in readout buffer:     " << formatRegValue(lEventsToRead);

	uint32_t lWordsToRead = aReadall ? lBufCount.value() : lEventsToRead * kEventSize;

	PDT_LOG(kNotice) << "Words to be read out in readout buffer: " << formatRegValue(lWordsToRead);
	
	if (!lWordsToRead) {
		PDT_LOG(kWarning) << "No words to be read out.";
	}
	
	auto lBufData = getNode("buf.data").readBlock(lWordsToRead);
	getClient().dispatch();

    return lBufData;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
EndpointNode::getDataBufferTable(bool aReadall, bool aPrint) const {

	std::stringstream lTable;
	auto lBufData = readDataBuffer(aReadall);

	std::vector<std::pair<std::string, uint32_t>> lBufferTable;

	uint32_t i=0;
	for (auto it=lBufData.begin(); it!=lBufData.end(); ++it, ++i) {
		std::stringstream lIndexStream;
		lIndexStream << std::setfill('0') << std::setw(4) << i;
		lBufferTable.push_back(std::make_pair(lIndexStream.str(), *it));
	}
	lTable << formatRegTable(lBufferTable, "Endpoint buffer", {"Word","Data"});
	
	if (aPrint) std::cout << lTable.str();
	return lTable.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double
EndpointNode::readClockFrequency() const {
	std::vector<double> lFrequencies = getNode<FrequencyCounterNode>("freq").measureFrequencies(1);
	return lFrequencies.at(0);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
EndpointNode::readVersion() const {
	auto lBufCount = getNode("version").read();
	getClient().dispatch();
	return lBufCount.value();
}
//-----------------------------------------------------------------------------
} // namespace pdt