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
	lStatus << this->get_io_node().get_pll_status();
	lStatus << this->get_master_node().get_status();
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
		this->get_master_node().sync_timestamp();

		// Enable spill interface
		this->get_master_node().enable_spill_interface();

		// Trigger interface configuration
		this->get_master_node().reset_external_triggers_endpoint();
		this->get_master_node().enable_external_triggers();
	}
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterMuxDesign<IO,MST>::reset(const std::string& aClockConfigFile) const {
	this->get_io_node().reset(-1, aClockConfigFile);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterMuxDesign<IO,MST>::switch_sfp_mux_channel(uint32_t aSFPID, bool aWaitForRttEptLock) const {
	this->get_io_node().switch_sfp_mux_channel(aSFPID);
	if (aWaitForRttEptLock) {
		this->get_master_node().enable_upstream_endpoint();
	}
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t MasterMuxDesign<IO,MST>::read_active_sfp_mux_channel() const {
	return this->get_io_node().read_active_sfp_mux_channel();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t MasterMuxDesign<IO,MST>::measureEndpointRTT(uint32_t aAddr, bool aControlSFP, uint32_t aSFPMUX) const {
	
	if (aControlSFP) {
		this->get_master_node().switch_endpoint_sfp(0x0, false);
		this->get_master_node().switch_endpoint_sfp(aAddr, true);
	}
	
	// set fanout rtt mux channel, and wait for fanout rtt ept to be in a good state
	this->switch_sfp_mux_channel(aSFPMUX, true);
		
	// gets master rtt ept in a good state, and sends echo command (due to second argument endpoint sfp is not controlled in this call, already done above)
	uint32_t lRTT = this->get_master_node().measureEndpointRTT(aAddr, false);

	if (aControlSFP) this->get_master_node().switch_endpoint_sfp(aAddr, false);
	return lRTT;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterMuxDesign<IO,MST>::apply_endpoint_delay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT, bool aControlSFP, uint32_t aSFPMUX) const {

	if (aMeasureRTT) {
		if (aControlSFP) {
			this->get_master_node().switch_endpoint_sfp(0x0, false);
			this->get_master_node().switch_endpoint_sfp(aAddr, true);
		}
		
		// set fanout rtt mux channel, and wait for fanout rtt ept to be in a good state, don't bother waiting for a good rtt endpoint, the next method call takes care of that
		this->switch_sfp_mux_channel(aSFPMUX, false);
	}
	
	this->get_master_node().apply_endpoint_delay(aAddr, aCDel, aFDel, aPDel, aMeasureRTT, false);

	if (aMeasureRTT && aControlSFP) this->get_master_node().switch_endpoint_sfp(aAddr, false);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
std::vector<uint32_t> MasterMuxDesign<IO,MST>::scan_sfp_mux() const {
	std::vector<uint32_t> lLockedChannels;
    
    // TODO will this be right for every fanout board, need to check the IO board
    uint32_t lNumberOfMuxChannels = 8;
    for (uint32_t i=0; i < lNumberOfMuxChannels; ++i) {
    	ERS_INFO("Scanning slot " << i);

    	try {
    		this->switch_sfp_mux_channel(i, true);	
    	} catch(...) {
    		ERS_INFO("Slot " << i << " not locked");
    	}
    	// TODO catch right except
    	
  		ERS_INFO("Slot " << i << " locked");
  		lLockedChannels.push_back(i);		
    }
    

    if (lLockedChannels.size()) {
    	ERS_LOG("Slots locked: " << vec_fmt(lLockedChannels));
    } else {
        ERS_LOG("No slots locked");
    }
    return lLockedChannels;
}
//-----------------------------------------------------------------------------
}