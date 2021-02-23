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
const EPT_TOP& TimingSystemManager<MST_TOP,EPT_TOP>::getEndpoint(uint32_t endpoint_id) const {
	return endpointHardware.at(endpoint_id).getNode<EPT_TOP>("");
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
void TimingSystemManager<MST_TOP,EPT_TOP>::resetPartition(uint32_t partition_id) const {
	getMaster(0).get_master_node().get_partition_node(partition_id).reset();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::configurePartition(uint32_t partition_id, uint32_t trigger_mask, bool enableSpillGate) const {
	getMaster(0).get_master_node().get_partition_node(partition_id).configure(trigger_mask, enableSpillGate);
	getMaster(0).get_master_node().get_partition_node(partition_id).enable();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::startPartition(uint32_t partition_id) const {
	getMaster(0).get_master_node().get_partition_node(partition_id).start();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::stopPartition(uint32_t partition_id) const {
	getMaster(0).get_master_node().get_partition_node(partition_id).stop();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
uint64_t TimingSystemManager<MST_TOP,EPT_TOP>::read_master_timestamp() const {
	return getMaster(0).read_master_timestamp();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
uint64_t TimingSystemManager<MST_TOP,EPT_TOP>::measure_endpoint_rtt(uint32_t address) const {
	return getMaster(0).get_master_node().measure_endpoint_rtt(address);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
uint64_t TimingSystemManager<MST_TOP,EPT_TOP>::measure_endpoint_rtt(const ActiveEndpointConfig& ept_config) const {
	uint32_t lEptAdr = ept_config.adr;
	uint32_t lEptFanout = ept_config.fanout;
	
	if (lEptFanout < 0) {
		// TODO warning?
		ERS_LOG( "Endpoint config contains an fanout ept mux value. This is a system without fanouts");
	}

	return getMaster(0).get_master_node().measure_endpoint_rtt(lEptAdr);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
std::vector<EndpointRTTResult> TimingSystemManager<MST_TOP,EPT_TOP>::performEndpointRTTScan() {

	std::vector<EndpointRTTResult> lEndpointResults;

	for (auto it = this->mExpectedEndpoints.begin(); it != this->mExpectedEndpoints.end(); ++it) {
		uint32_t lRTT = this->measure_endpoint_rtt(it->second);
		
		if (lRTT > this->mMaxMeasuredRTT) this->mMaxMeasuredRTT = lRTT;
		
		lEndpointResults.push_back(EndpointRTTResult(it->second, lRTT));
	}
	return lEndpointResults;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP>
void TimingSystemManager<MST_TOP,EPT_TOP>::apply_endpoint_delays(uint32_t measure_rtt) const {

	for (auto it = this->mExpectedEndpoints.begin(); it != this->mExpectedEndpoints.end(); ++it) {
		std::string lEptIdD = it->second.id;
		uint32_t lCDelay = it->second.cdelay;
		uint32_t lFDelay = it->second.fdelay;

		uint32_t lRttBefore;

		if (measure_rtt) lRttBefore = measure_endpoint_rtt(it->second);
		
		getMaster(0).get_master_node().apply_endpoint_delay(it->second, false);

		if (measure_rtt) {
			uint32_t lRttAfter = measure_endpoint_rtt(it->second);
			ERS_INFO(lEptIdD << " delay adjustment; cdel: " << lCDelay << ", fdel: " << lFDelay);
			ERS_INFO("before RTT: " << std::dec << lRttBefore << ", after RTT: " << lRttAfter);
		}
	}
}
//-----------------------------------------------------------------------------

}