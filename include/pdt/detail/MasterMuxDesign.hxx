#include <string>
#include <sstream>

namespace dunedaq::pdt
{


// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO, class MST>
uhal::Node* MasterMuxDesign<IO,MST>::clone() const { 
    return new MasterMuxDesign<IO,MST> ( static_cast<const MasterMuxDesign<IO,MST>&> ( *this ) );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
MasterMuxDesign<IO,MST>::MasterMuxDesign(const uhal::Node& aNode) : MasterDesign<IO,MST>(aNode) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
MasterMuxDesign<IO,MST>::~MasterMuxDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
std::string MasterMuxDesign<IO,MST>::get_status(bool aPrint) const {
	std::stringstream lStatus;
	lStatus << this->getIONode().getPLLStatus();
	lStatus << this->getMasterNode().get_status();
	// TODO mux specific status
	if (aPrint) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterMuxDesign<IO,MST>::configure() const {
	// fanout mode hard-coded, to be passed in as parameter in future
	uint32_t lFanoutMode = 0;
	
	// Hard reset
	this->reset();

	if (!lFanoutMode) {
		// Set timestamp to current time
		this->getMasterNode().syncTimestamp();

		// Enable spill interface
		this->getMasterNode().enableSpillInterface();

		// Trigger interface configuration
		this->getMasterNode().resetExternalTriggersEndpoint();
		this->getMasterNode().enableExternalTriggers();
	}
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterMuxDesign<IO,MST>::reset(const std::string& aClockConfigFile) const {
	this->getIONode().reset(-1, aClockConfigFile);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterMuxDesign<IO,MST>::switchSFPMUXChannel(uint32_t aSFPID, bool aWaitForRttEptLock) const {
	this->getIONode().switchSFPMUXChannel(aSFPID);
	if (aWaitForRttEptLock) {
		this->getMasterNode().enableUpstreamEndpoint();
	}
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t MasterMuxDesign<IO,MST>::readActiveSFPMUXChannel() const {
	return this->getIONode().readActiveSFPMUXChannel();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t MasterMuxDesign<IO,MST>::measureEndpointRTT(uint32_t aAddr, bool aControlSFP, uint32_t aSFPMUX) const {
	
	if (aControlSFP) {
		this->getMasterNode().switchEndpointSFP(0x0, false);
		this->getMasterNode().switchEndpointSFP(aAddr, true);
	}
	
	// set fanout rtt mux channel, and wait for fanout rtt ept to be in a good state
	this->switchSFPMUXChannel(aSFPMUX, true);
		
	// gets master rtt ept in a good state, and sends echo command (due to second argument endpoint sfp is not controlled in this call, already done above)
	uint32_t lRTT = this->getMasterNode().measureEndpointRTT(aAddr, false);

	if (aControlSFP) this->getMasterNode().switchEndpointSFP(aAddr, false);
	return lRTT;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterMuxDesign<IO,MST>::applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT, bool aControlSFP, uint32_t aSFPMUX) const {

	if (aMeasureRTT) {
		if (aControlSFP) {
			this->getMasterNode().switchEndpointSFP(0x0, false);
			this->getMasterNode().switchEndpointSFP(aAddr, true);
		}
		
		// set fanout rtt mux channel, and wait for fanout rtt ept to be in a good state, don't bother waiting for a good rtt endpoint, the next method call takes care of that
		this->switchSFPMUXChannel(aSFPMUX, false);
	}
	
	this->getMasterNode().applyEndpointDelay(aAddr, aCDel, aFDel, aPDel, aMeasureRTT, false);

	if (aMeasureRTT && aControlSFP) this->getMasterNode().switchEndpointSFP(aAddr, false);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
std::vector<uint32_t> MasterMuxDesign<IO,MST>::scanSFPMUX() const {
	std::vector<uint32_t> lLockedChannels;
    
    // TODO will this be right for every fanout board, need to check the IO board
    uint32_t lNumberOfMuxChannels = 8;
    for (uint32_t i=0; i < lNumberOfMuxChannels; ++i) {
    	ERS_INFO("Scanning slot " << i);

    	try {
    		this->switchSFPMUXChannel(i, true);	
    	} catch(...) {
    		ERS_INFO("Slot " << i << " not locked");
    	}
    	// TODO catch right except
    	
  		ERS_INFO("Slot " << i << " locked");
  		lLockedChannels.push_back(i);		
    }
    

    if (lLockedChannels.size()) {
    	ERS_LOG("Slots locked: " << vecFmt(lLockedChannels));
    } else {
        ERS_LOG("No slots locked");
    }
    return lLockedChannels;
}
//-----------------------------------------------------------------------------
}