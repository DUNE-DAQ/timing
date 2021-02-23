#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HXX_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HXX_

#include <string>
#include <sstream>

namespace dunedaq::pdt
{


//-----------------------------------------------------------------------------
template< class IO>
TopDesign<IO>::TopDesign(const uhal::Node& node) : TimingNode(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
TopDesign<IO>::~TopDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
void TopDesign<IO>::soft_reset() const {
	get_io_node().soft_reset();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
void TopDesign<IO>::reset(const std::string& clock_config_file) const {
	get_io_node().reset(clock_config_file);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
const IO& TopDesign<IO>::get_io_node() const {
	return uhal::Node::getNode<IO>("io");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
const EndpointNode& TopDesign<IO>::get_endpoint_node(uint32_t ept_id) const {
	const std::string nodeName = "endpoint" + std::to_string(ept_id);
	return uhal::Node::getNode<EndpointNode>(nodeName);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
std::string TopDesign<IO>::get_hardware_info(bool print_out) const {
	auto lInfo = get_io_node().get_hardware_info();
	if (print_out) std::cout << lInfo;
	return lInfo;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
uint32_t TopDesign<IO>::get_number_of_endpoint_nodes() const {
	std::string lRegexString = "endpoint[0-9]+";
	return uhal::Node::getNodes(lRegexString).size();
}
//-----------------------------------------------------------------------------
}

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HXX_