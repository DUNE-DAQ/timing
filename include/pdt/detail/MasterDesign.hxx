#include <string>
#include <sstream>

namespace dunedaq::pdt
{

//-----------------------------------------------------------------------------
template<class IO, class MST>
MasterDesign<IO,MST>::MasterDesign(const uhal::Node& aNode) : TopDesign<IO>(aNode) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
MasterDesign<IO,MST>::~MasterDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< class IO, class MST>
const MST& MasterDesign<IO,MST>::get_master_node() const {
	return uhal::Node::getNode<MST>("master_top");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
uint64_t MasterDesign<IO,MST>::read_master_timestamp() const {
	return get_master_node().read_timestamp();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t MasterDesign<IO,MST>::measureEndpointRTT(uint32_t aAddr, bool aControlSFP) const  {
	return get_master_node().measureEndpointRTT(aAddr, aControlSFP);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterDesign<IO,MST>::apply_endpoint_delay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT, bool aControlSFP) const {
	get_master_node().apply_endpoint_delay(aAddr, aCDel, aFDel, aPDel, aMeasureRTT, aControlSFP);
}
//-----------------------------------------------------------------------------
}