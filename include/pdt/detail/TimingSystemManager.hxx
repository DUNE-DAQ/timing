namespace dunedaq::pdt
{


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
TimingSystemManager<MST_TOP,EPT_TOP>::TimingSystemManager(std::string cf) : TimingSystemManagerBase(cf) {
	connectionManager = new uhal::ConnectionManager(connectionsFile);

	// set up master devices
	for (auto it=masterHardwareNames.begin(); it != masterHardwareNames.end(); ++it) {
		masterHardware.push_back( connectionManager->getDevice(*it) );
	}

	// set up endpoint devices
	for (auto it=endpointHardwareNames.begin(); it != endpointHardwareNames.end(); ++it) {
		endpointHardware.push_back( connectionManager->getDevice(*it) );
	}
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
TimingSystemManager<MST_TOP,EPT_TOP>::~TimingSystemManager() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
const MST_TOP& TimingSystemManager<MST_TOP,EPT_TOP>::getMaster(uint32_t aMstId) const {
	return masterHardware.at(aMstId).getNode<MST_TOP>("");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
const EPT_TOP& TimingSystemManager<MST_TOP,EPT_TOP>::getEndpoint(uint32_t aEptId) const {
	return endpointHardware.at(aEptId).getNode<EPT_TOP>("");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::printSystemStatus() const {
	std::cout << "Timing master status" << std::endl;
	for (uint32_t i = 0; i < getNumberOfMasters(); ++i) getMaster(i).get_status(true);

	for (uint32_t i = 0; i < this->getNumberOfEndpoints(); ++i) {
		std::cout << "Timing endpoint " << i << " status" << std::endl;
		this->getEndpoint(i).get_status(true);
	}
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::resetSystem() const {
	for (uint32_t i = 0; i < getNumberOfMasters(); ++i) getMaster(i).reset();
	for (uint32_t i = 0; i < getNumberOfEndpoints(); ++i) getEndpoint(i).reset();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::configureSystem() const {
	for (uint32_t i = 0; i < getNumberOfMasters(); ++i) getMaster(i).configure();
	// apply endpoint delays
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::resetPartition(uint32_t aPartID) const {
	getMaster(0).getMasterNode().getPartitionNode(aPartID).reset();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::configurePartition(uint32_t aPartID, uint32_t aTrigMask, bool aEnableSpillGate) const {
	getMaster(0).getMasterNode().getPartitionNode(aPartID).configure(aTrigMask, aEnableSpillGate);
	getMaster(0).getMasterNode().getPartitionNode(aPartID).enable();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::startPartition(uint32_t aPartID) const {
	getMaster(0).getMasterNode().getPartitionNode(aPartID).start();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::stopPartition(uint32_t aPartID) const {
	getMaster(0).getMasterNode().getPartitionNode(aPartID).stop();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
uint64_t TimingSystemManager<MST_TOP,EPT_TOP>::readMasterTimestamp() const {
	return getMaster(0).readMasterTimestamp();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
uint64_t TimingSystemManager<MST_TOP,EPT_TOP>::measureEndpointRTT(uint32_t aAddr) const {
	return getMaster(0).getMasterNode().measureEndpointRTT(aAddr);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
uint64_t TimingSystemManager<MST_TOP,EPT_TOP>::measureEndpointRTT(const ActiveEndpointConfig& aEptConfig) const {
	uint32_t lEptAdr = aEptConfig.adr;
	uint32_t lEptFanout = aEptConfig.fanout;
	
	if (lEptFanout < 0) {
		// TODO warning?
		ERS_LOG( "Endpoint config contains an fanout ept mux value. This is a system without fanouts");
	}

	return getMaster(0).getMasterNode().measureEndpointRTT(lEptAdr);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
std::vector<EndpointRTTResult> TimingSystemManager<MST_TOP,EPT_TOP>::performEndpointRTTScan() {

	std::vector<EndpointRTTResult> lEndpointResults;

	for (auto it = this->mExpectedEndpoints.begin(); it != this->mExpectedEndpoints.end(); ++it) {
		uint32_t lRTT = this->measureEndpointRTT(it->second);
		
		if (lRTT > this->mMaxMeasuredRTT) this->mMaxMeasuredRTT = lRTT;
		
		lEndpointResults.push_back(EndpointRTTResult(it->second, lRTT));
	}
	return lEndpointResults;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::applyEndpointDelays(uint32_t aMeasureRTT) const {

	for (auto it = this->mExpectedEndpoints.begin(); it != this->mExpectedEndpoints.end(); ++it) {
		std::string lEptIdD = it->second.id;
		uint32_t lCDelay = it->second.cdelay;
		uint32_t lFDelay = it->second.fdelay;

		uint32_t lRttBefore;

		if (aMeasureRTT) lRttBefore = measureEndpointRTT(it->second);
		
		getMaster(0).getMasterNode().applyEndpointDelay(it->second, false);

		if (aMeasureRTT) {
			uint32_t lRttAfter = measureEndpointRTT(it->second);
			ERS_INFO(lEptIdD << " delay adjustment; cdel: " << lCDelay << ", fdel: " << lFDelay);
			ERS_INFO("before RTT: " << std::dec << lRttBefore << ", after RTT: " << lRttAfter);
		}
	}
}
//-----------------------------------------------------------------------------

}