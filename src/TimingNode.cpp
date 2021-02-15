#include "pdt/TimingNode.hpp"

namespace dunedaq {
namespace pdt {

//-----------------------------------------------------------------------------
TimingNode::TimingNode(const uhal::Node& aNode) : uhal::Node(aNode) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
TimingNode::~TimingNode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::map<std::string,uhal::ValWord<uint32_t>>
TimingNode::read_sub_nodes(const uhal::Node& aNode, bool dispatch) const {
	auto lNodeNames = aNode.getNodes();
	
	std::map<std::string,uhal::ValWord<uint32_t>> lNodeNameValuePairs;

	for (auto it=lNodeNames.begin(); it != lNodeNames.end(); ++it) lNodeNameValuePairs[*it] = aNode.getNode(*it).read();
	if (dispatch) getClient().dispatch();
	return lNodeNameValuePairs;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TimingNode::reset_sub_nodes(const uhal::Node& aNode, uint32_t aValue, bool dispatch) const {

	auto lNodeNames = aNode.getNodes();

	for (auto it=lNodeNames.begin(); it != lNodeNames.end(); ++it) aNode.getNode(*it).write(aValue);

    if (dispatch) getClient().dispatch();
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq