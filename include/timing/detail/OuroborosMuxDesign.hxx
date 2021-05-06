#include <string>
#include <sstream>

namespace dunedaq::timing
{

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node* OuroborosMuxDesign<IO>::clone() const {
    return new OuroborosMuxDesign<IO> ( static_cast<const OuroborosMuxDesign<IO>&> ( *this ) );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
OuroborosMuxDesign<IO>::OuroborosMuxDesign(const uhal::Node& node) : TopDesign<IO>(node), MasterMuxDesign<IO,PDIMasterNode>(node), EndpointDesign<IO>(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
OuroborosMuxDesign<IO>::~OuroborosMuxDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
std::string OuroborosMuxDesign<IO>::get_status(bool print_out) const {
	std::stringstream lStatus;
	lStatus << this->get_io_node().get_pll_status();
	lStatus << this->get_master_node().get_status();
	if (print_out) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
void OuroborosMuxDesign<IO>::configure() const {
	
	// Hard resets
	this->reset();

	// Set timestamp to current time
	this->get_master_node().sync_timestamp();

	// Enable spill interface
	this->get_master_node().enable_spill_interface();
}
//-----------------------------------------------------------------------------


}