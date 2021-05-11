#include <string>
#include <sstream>

namespace dunedaq::timing
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
MasterMuxDesign<IO,MST>::MasterMuxDesign(const uhal::Node& node) : TopDesign<IO>(node), MasterDesign<IO,MST>(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
MasterMuxDesign<IO,MST>::~MasterMuxDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
std::string MasterMuxDesign<IO,MST>::get_status(bool print_out) const {
	std::stringstream lStatus;
	lStatus << this->get_io_node().get_pll_status();
	lStatus << this->get_master_node().get_status();
	// TODO mux specific status
	if (print_out) std::cout << lStatus.str();
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
	}
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterMuxDesign<IO,MST>::reset(const std::string& clock_config_file) const {
	this->get_io_node().reset(-1, clock_config_file);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterMuxDesign<IO,MST>::switch_sfp_mux_channel(uint32_t sfp_id, bool wait_for_rtt_ept_lock) const {
	this->get_io_node().switch_sfp_mux_channel(sfp_id);
	if (wait_for_rtt_ept_lock) {
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
uint32_t MasterMuxDesign<IO,MST>::measure_endpoint_rtt(uint32_t address, bool control_sfp, uint32_t sfp_mux) const {
	
	if (control_sfp) {
		this->get_master_node().switch_endpoint_sfp(0x0, false);
		this->get_master_node().switch_endpoint_sfp(address, true);
	}
	
	// set fanout rtt mux channel, and do not wait for fanout rtt ept to be in a good state
	this->switch_sfp_mux_channel(sfp_mux, false);

	// sleep for a short time, otherwise the rtt endpoint will not get state to 0x8 in time
	millisleep(200);
		
	// gets master rtt ept in a good state, and sends echo command (due to second argument endpoint sfp is not controlled in this call, already done above)
	uint32_t lRTT = this->get_master_node().measure_endpoint_rtt(address, false);

	if (control_sfp) this->get_master_node().switch_endpoint_sfp(address, false);
	return lRTT;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterMuxDesign<IO,MST>::apply_endpoint_delay(uint32_t address, uint32_t coarse_delay, uint32_t fine_delay, uint32_t phase_delay, bool measure_rtt, bool control_sfp, uint32_t sfp_mux) const {

	if (measure_rtt) {
		if (control_sfp) {
			this->get_master_node().switch_endpoint_sfp(0x0, false);
			this->get_master_node().switch_endpoint_sfp(address, true);
		}
		
		// set fanout rtt mux channel, and wait for fanout rtt ept to be in a good state, don't bother waiting for a good rtt endpoint, the next method call takes care of that
		this->switch_sfp_mux_channel(sfp_mux, false);
	}
	
	this->get_master_node().apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay, measure_rtt, false);

	if (measure_rtt && control_sfp) this->get_master_node().switch_endpoint_sfp(address, false);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
std::vector<uint32_t> MasterMuxDesign<IO,MST>::scan_sfp_mux() const {
	std::vector<uint32_t> lLockedChannels;
    
    // TODO will this be right for every fanout board, need to check the IO board
    uint32_t lNumberOfMuxChannels = 8;
    for (uint32_t i=0; i < lNumberOfMuxChannels; ++i) {
    	TLOG_DEBUG(0) << "Scanning slot " << i;

    	try {
    		this->switch_sfp_mux_channel(i, true);	
    	} catch(...) {
    		TLOG_DEBUG(0) << "Slot " << i << " not locked";
    	}
    	// TODO catch right except
    	
  		TLOG_DEBUG(0) << "Slot " << i << " locked";
  		lLockedChannels.push_back(i);		
    }
    

    if (lLockedChannels.size()) {
    	TLOG() << "Slots locked: " << vec_fmt(lLockedChannels);
    } else {
        TLOG() << "No slots locked";
    }
    return lLockedChannels;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
template <class T>
void MasterMuxDesign<IO,MST>::get_info(T& data) const {
	this->get_master_node().get_info(data.master_data);
	this->get_io_node().get_info(data.hardware_data);
}
//-----------------------------------------------------------------------------
}