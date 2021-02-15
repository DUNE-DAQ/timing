#include "pdt/FanoutIONode.hpp"

namespace dunedaq {
namespace pdt {

//-----------------------------------------------------------------------------
FanoutIONode::FanoutIONode(const uhal::Node& aNode, std::string aUIDI2CBus, std::string aUIDI2CDevice, std::string aPLLI2CBus, std::string aPLLI2CDevice, std::vector<std::string> aClockNames, std::vector<std::string> aSFPI2CBuses) :
	IONode(aNode, aUIDI2CBus, aUIDI2CDevice, aPLLI2CBus, aPLLI2CDevice, aClockNames, aSFPI2CBuses) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
FanoutIONode::~FanoutIONode() {
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq