namespace dunedaq::pdt
{


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::TimingSystemWithFanoutManager(std::string cf) :
	TimingSystemManager<MST_TOP,EPT_TOP>(cf),
	fanoutHardwareNames({"PROD_FANOUT_0"}) 
{
	// set up fanout devices
	for (auto it=fanoutHardwareNames.begin(); it != fanoutHardwareNames.end(); ++it) {
		fanoutHardware.push_back( this->connectionManager->getDevice(*it) );
	}
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::~TimingSystemWithFanoutManager() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
uint32_t TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::getNumberOfFanouts() const {
	return fanoutHardware.size();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
const FAN_TOP& TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::getFanout(uint32_t aFanoutId) const {
	return fanoutHardware.at(aFanoutId).getNode<FAN_TOP>("");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::printSystemStatus() const {
	std::cout << "Timing master status" << std::endl;
	for (uint32_t i = 0; i < this->getNumberOfMasters(); ++i) this->getMaster(i).get_status(true);	


	std::cout << "Timing fanout status" << std::endl;
	for (uint32_t i = 0; i < getNumberOfFanouts(); ++i) getFanout(i).get_status(true);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::resetSystem() const {
	for (uint32_t i = 0; i < this->getNumberOfMasters(); ++i) this->getMaster(i).reset();
	for (uint32_t i = 0; i < getNumberOfFanouts(); ++i) getFanout(i).reset();
	for (uint32_t i = 0; i < this->getNumberOfEndpoints(); ++i) this->getEndpoint(i).reset();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::configureSystem() const {
	for (uint32_t i = 0; i < this->getNumberOfMasters(); ++i) this->getMaster(i).configure();
	for (uint32_t i = 0; i < getNumberOfFanouts(); ++i) getFanout(i).configure();
	// apply endpoint delays
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::resetPartition(uint32_t aPartID) const {
	this->getMaster(0).getMasterNode().getPartitionNode(aPartID).reset();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::configurePartition(uint32_t aPartID, uint32_t aTrigMask, bool aEnableSpillGate) const {
	this->getMaster(0).getMasterNode().getPartitionNode(aPartID).configure(aTrigMask, aEnableSpillGate);
	this->getMaster(0).getMasterNode().getPartitionNode(aPartID).enable();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::startPartition(uint32_t aPartID) const {
	this->getMaster(0).getMasterNode().getPartitionNode(aPartID).start();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::stopPartition(uint32_t aPartID) const {
	this->getMaster(0).getMasterNode().getPartitionNode(aPartID).stop();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
uint64_t TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::readMasterTimestamp() const {
	return this->getMaster(0).readMasterTimestamp();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
uint64_t TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::measureEndpointRTT(uint32_t aAddr, int32_t aFanout, uint32_t aMux) const {
	
	uint32_t lRTT;
		
	this->getMaster(0).getMasterNode().switchEndpointSFP(0x0, false);
	this->getMaster(0).getMasterNode().switchEndpointSFP(aAddr, true);

	// TODO check which fanout is active, and switch if necessary
		
	// set fanout rtt mux channel, and wait for fanout rtt ept to be in a good state
	if (aFanout >= 0) getFanout(aFanout).switchSFPMUXChannel(aMux, true);
		
	// gets master rtt ept in a good state, and sends echo command (due to second argument endpoint sfp is not controlled in this call, already done above)
	lRTT = this->getMaster(0).getMasterNode().measureEndpointRTT(aAddr, false);

	this->getMaster(0).getMasterNode().switchEndpointSFP(aAddr, false);
		
	return lRTT;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
uint64_t TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::measureEndpointRTT(uint32_t aAddr) const {
	return this->measureEndpointRTT(aAddr, -1, 0);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
uint64_t TimingSystemWithFanoutManager<MST_TOP,EPT_TOP,FAN_TOP>::measureEndpointRTT(const ActiveEndpointConfig& aEptConfig) const {
	uint32_t lEptAdr = aEptConfig.adr;
	uint32_t lEptFanout = aEptConfig.fanout;
	uint32_t lEptMux = aEptConfig.mux;	

	return this->measureEndpointRTT(lEptAdr, lEptFanout, lEptMux);
}
//-----------------------------------------------------------------------------

}