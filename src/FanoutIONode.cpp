#include "pdt/FanoutIONode.hpp"

namespace dunedaq {
namespace pdt {

//-----------------------------------------------------------------------------
FanoutIONode::FanoutIONode(const uhal::Node& node, std::string uid_i2c_bus, std::string uid_i2c_device, std::string pll_i2c_bus, std::string pll_i2c_device, std::vector<std::string> clock_names, std::vector<std::string> sfp_i2c_buses) :
	IONode(node, uid_i2c_bus, uid_i2c_device, pll_i2c_bus, pll_i2c_device, clock_names, sfp_i2c_buses) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
FanoutIONode::~FanoutIONode() {
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq