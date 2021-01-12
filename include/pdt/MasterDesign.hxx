#include <string>
#include <sstream>

namespace pdt
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
const MST& MasterDesign<IO,MST>::getMasterNode() const {
	return uhal::Node::getNode<MST>("master_top");
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
uint64_t MasterDesign<IO,MST>::readMasterTimestamp() const {
	return getMasterNode().readTimestamp();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t MasterDesign<IO,MST>::measureEndpointRTT(uint32_t aAddr, bool aControlSFP) const  {
	return getMasterNode().measureEndpointRTT(aAddr, aControlSFP);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void MasterDesign<IO,MST>::applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT, bool aControlSFP) const {
	getMasterNode().applyEndpointDelay(aAddr, aCDel, aFDel, aPDel, aMeasureRTT, aControlSFP);
}
//-----------------------------------------------------------------------------
}