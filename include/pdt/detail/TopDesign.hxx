#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HXX_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HXX_

#include <string>
#include <sstream>

namespace dunedaq::pdt
{


//-----------------------------------------------------------------------------
template< class IO>
TopDesign<IO>::TopDesign(const uhal::Node& aNode) : TimingNode(aNode) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
TopDesign<IO>::~TopDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
void TopDesign<IO>::softReset() const {
	getIONode().softReset();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
void TopDesign<IO>::reset(const std::string& aClockConfigFile) const {
	getIONode().reset(aClockConfigFile);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
const IO& TopDesign<IO>::getIONode() const {
	return uhal::Node::getNode<IO>("io");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
const EndpointNode& TopDesign<IO>::getEndpointNode(uint32_t ept_id) const {
	const std::string nodeName = "endpoint" + std::to_string(ept_id);
	return uhal::Node::getNode<EndpointNode>(nodeName);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
std::string TopDesign<IO>::getHardwareInfo(bool aPrint) const {
	auto lInfo = getIONode().getHardwareInfo();
	if (aPrint) std::cout << lInfo;
	return lInfo;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
uint32_t TopDesign<IO>::getNumberOfEndpointNodes() const {
	std::string lRegexString = "endpoint[0-9]+";
	return uhal::Node::getNodes(lRegexString).size();
}
//-----------------------------------------------------------------------------
}

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HXX_