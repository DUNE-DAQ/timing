#include <string>
#include <sstream>

namespace dunedaq::timing
{

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node* PDIMasterDesign<IO>::clone() const {
    return new PDIMasterDesign<IO> ( static_cast<const PDIMasterDesign<IO>&> ( *this ) );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
PDIMasterDesign<IO>::PDIMasterDesign(const uhal::Node& node) : MasterDesign<IO,PDIMasterNode>(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
PDIMasterDesign<IO>::~PDIMasterDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
std::string PDIMasterDesign<IO>::get_status(bool print_out) const {
	std::stringstream lStatus;
	lStatus << this->get_io_node().get_pll_status();
	lStatus << this->get_master_node().get_status();
	if (print_out) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
void PDIMasterDesign<IO>::configure() const {
	
	// Hard resets
	this->reset();

	// Set timestamp to current time
	this->get_master_node().sync_timestamp();

	// Enable spill interface
	this->get_master_node().enable_spill_interface();

	// Trigger interface configuration
	this->get_master_node().reset_external_triggers_endpoint();
	this->get_master_node().enable_external_triggers();
}
//-----------------------------------------------------------------------------

}