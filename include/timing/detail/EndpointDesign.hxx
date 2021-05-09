#include <string>
#include <sstream>

namespace dunedaq::timing
{

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template< class IO>
uhal::Node* EndpointDesign<IO>::clone() const { 
    return new EndpointDesign<IO> ( static_cast<const EndpointDesign<IO>&> ( *this ) );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
EndpointDesign<IO>::EndpointDesign(const uhal::Node& node) : TopDesign<IO>(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
EndpointDesign<IO>::~EndpointDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
std::string EndpointDesign<IO>::get_status(bool print_out) const {
	std::stringstream lStatus;
	lStatus << this->get_io_node().get_pll_status();
	uint32_t lNumberOfEndpointNodes = EndpointDesign<IO>::get_number_of_endpoint_nodes();
	for (uint32_t i=0; i < lNumberOfEndpointNodes; ++i) {
		lStatus << "Endpoint node " << i << " status" << std::endl;
		lStatus << this->get_endpoint_node(i).get_status();
	}
	if (print_out) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
template <class T>
void EndpointDesign<IO>::get_info(T& data) const {
	this->get_endpoint_node(0).get_info(data.firmware_data);
	this->get_io_node().get_info(data.hardware_data);
}
//-----------------------------------------------------------------------------

}