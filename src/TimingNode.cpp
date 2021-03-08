#include "timing/TimingNode.hpp"

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
TimingNode::TimingNode(const uhal::Node& node) : uhal::Node(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
TimingNode::~TimingNode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::map<std::string,uhal::ValWord<uint32_t>>
TimingNode::read_sub_nodes(const uhal::Node& node, bool dispatch) const {
	auto lNodeNames = node.getNodes();
	
	std::map<std::string,uhal::ValWord<uint32_t>> lNodeNameValuePairs;

	for (auto it=lNodeNames.begin(); it != lNodeNames.end(); ++it) lNodeNameValuePairs[*it] = node.getNode(*it).read();
	if (dispatch) getClient().dispatch();
	return lNodeNameValuePairs;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
TimingNode::reset_sub_nodes(const uhal::Node& node, uint32_t aValue, bool dispatch) const {

	auto lNodeNames = node.getNodes();

	for (auto it=lNodeNames.begin(); it != lNodeNames.end(); ++it) node.getNode(*it).write(aValue);

    if (dispatch) getClient().dispatch();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq