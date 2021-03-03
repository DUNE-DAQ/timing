#include <string>
#include <sstream>

namespace dunedaq::pdt
{


// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO, class MST>
uhal::Node* FanoutDesign<IO,MST>::clone() const { 
    return new FanoutDesign<IO,MST> ( static_cast<const FanoutDesign<IO,MST>&> ( *this ) );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
FanoutDesign<IO,MST>::FanoutDesign(const uhal::Node& node) : MasterMuxDesign<IO,MST>(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
FanoutDesign<IO,MST>::~FanoutDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
std::string FanoutDesign<IO,MST>::get_status(bool print_out) const {
	std::stringstream lStatus;
	lStatus << this->get_io_node().get_pll_status();
	lStatus << this->get_master_node().get_status();
	// TODO fanout specific status
	if (print_out) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void FanoutDesign<IO,MST>::configure() const {
	// fanout mode hard-coded, to be passed in as parameter in future
	uint32_t lFanoutMode = 0;
	
	// Hard reset
	this->reset(lFanoutMode);

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
void FanoutDesign<IO,MST>::reset(uint32_t fanout_mode, const std::string& clock_config_file) const {
	this->get_io_node().reset(fanout_mode, clock_config_file);
	// 0 - fanout mode, outgoing data comes from sfp
	// 1 - standalone mode, outgoing data comes from local master
	uhal::Node::getNode("switch.csr.ctrl.master_src").write(fanout_mode);
	uhal::Node::getClient().dispatch();
}
//-----------------------------------------------------------------------------


////-----------------------------------------------------------------------------
//template<class IO, class MST>
//void FanoutDesign<IO,MST>::reset(const std::string& clock_config_file) const {
//	this->reset(0, clock_config_file);
//}
////-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t FanoutDesign<IO,MST>::measure_endpoint_rtt(uint32_t address, bool control_sfp, uint32_t sfp_mux) const {
	auto lFanoutMode = uhal::Node::getNode("switch.csr.ctrl.master_src").read();
	uhal::Node::getClient().dispatch();

	if (!lFanoutMode.value()) {
		std::ostringstream lMsg;
        lMsg << " Fanout unit " << uhal::Node::getId() << " is in fanout mode. Measure endpoint RTT should be called from master device.";
        throw UnsupportedFunction(ERS_HERE, lMsg.str());
	}
	return MasterMuxDesign<IO,MST>::measure_endpoint_rtt(address,control_sfp,sfp_mux);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void FanoutDesign<IO,MST>::apply_endpoint_delay(uint32_t address, uint32_t coarse_delay, uint32_t fine_delay, uint32_t phase_delay, bool measure_rtt, bool control_sfp, uint32_t sfp_mux) const {
	auto lFanoutMode = uhal::Node::getNode("switch.csr.ctrl.master_src").read();
	uhal::Node::getClient().dispatch();

	if (!lFanoutMode.value()) {
		std::ostringstream lMsg;
        lMsg << "Fanout unit " << uhal::Node::getId() << " is in fanout mode. Apply endpoint delay should be called from master device." ;
        throw UnsupportedFunction(ERS_HERE, lMsg.str());
	}
	MasterMuxDesign<IO,MST>::apply_endpoint_delay(address,coarse_delay,fine_delay,phase_delay,measure_rtt,control_sfp,sfp_mux);
}
//-----------------------------------------------------------------------------

}