#include <string>
#include <sstream>

namespace pdt
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
EndpointDesign<IO>::EndpointDesign(const uhal::Node& aNode) : TopDesign<IO>(aNode) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
EndpointDesign<IO>::~EndpointDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
std::string EndpointDesign<IO>::getStatus(bool aPrint) const {
	std::stringstream lStatus;
	lStatus << this->getIONode().getPLLStatus();
	uint32_t lNumberOfEndpointNodes = EndpointDesign<IO>::getNumberOfEndpointNodes();
	for (uint32_t i=0; i < lNumberOfEndpointNodes; ++i) {
		lStatus << "Endpoint node " << i << " status" << std::endl;
		lStatus << this->getEndpointNode(i).getStatus();
	}
	if (aPrint) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO>
void EndpointDesign<IO>::enable(uint32_t aEptId) const {
	 this->getEndpointNode(aEptId).enable();
}
//-----------------------------------------------------------------------------

}