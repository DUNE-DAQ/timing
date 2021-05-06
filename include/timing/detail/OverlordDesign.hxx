#include <string>
#include <sstream>

namespace dunedaq::timing
{

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node* OverlordDesign<IO>::clone() const {
    return new OverlordDesign<IO> ( static_cast<const OverlordDesign<IO>&> ( *this ) );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
OverlordDesign<IO>::OverlordDesign(const uhal::Node& node) : TopDesign<IO>(node), MasterDesign<IO,PDIMasterNode>(node), EndpointDesign<IO>(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
OverlordDesign<IO>::~OverlordDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
std::string OverlordDesign<IO>::get_status(bool print_out) const {
	std::stringstream lStatus;
	lStatus << this->get_io_node().get_pll_status();
	lStatus << this->get_master_node().get_status();
	if (print_out) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
void OverlordDesign<IO>::configure() const {
	
	// Hard resets
	this->reset();

	// Set timestamp to current time
	this->get_master_node().sync_timestamp();

	// Enable spill interface
	this->get_master_node().enable_spill_interface();

	// Trigger interface configuration
	this->reset_external_triggers_endpoint();
	this->enable_external_triggers();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
void OverlordDesign<IO>::reset_external_triggers_endpoint() const {
    //this->getNode<timing::TriggerReceiverNode>("trig").reset();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
void OverlordDesign<IO>::enable_external_triggers() const {
    //uhal::getNode<TriggerReceiverNode>("trig").enable_triggers();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
void OverlordDesign<IO>::disable_external_triggers() const {
    //uhal::getNode<TriggerReceiverNode>("trig").disable_triggers();
}
//-----------------------------------------------------------------------------

}