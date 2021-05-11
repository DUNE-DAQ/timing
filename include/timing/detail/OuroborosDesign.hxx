#include <string>
#include <sstream>

namespace dunedaq::timing
{

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node* OuroborosDesign<IO>::clone() const {
    return new OuroborosDesign<IO> ( static_cast<const OuroborosDesign<IO>&> ( *this ) );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
OuroborosDesign<IO>::OuroborosDesign(const uhal::Node& node) : TopDesign<IO>(node), MasterDesign<IO,PDIMasterNode>(node), EndpointDesign<IO>(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
OuroborosDesign<IO>::~OuroborosDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
std::string OuroborosDesign<IO>::get_status(bool print_out) const {
	std::stringstream lStatus;
	lStatus << this->get_io_node().get_pll_status();
	lStatus << this->get_master_node().get_status();
	lStatus << this->get_endpoint_node(0).get_status();
	if (print_out) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
void OuroborosDesign<IO>::configure() const {
	
	// Hard resets
	this->reset();

	// Set timestamp to current time
	this->get_master_node().sync_timestamp();

	// Enable spill interface
	this->get_master_node().enable_spill_interface();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
template <class T>
void OuroborosDesign<IO>::get_info(T& data) const {
	this->get_master_node().get_info(data.master_data);
	this->get_io_node().get_info(data.hardware_data);
	//this->get_endpoint_node(0).get_info(data);
}
//-----------------------------------------------------------------------------


}