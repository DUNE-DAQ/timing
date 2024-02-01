/**
 * @file SFPMuxIONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/SFPMuxIONode.hpp"

#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
SFPMuxIONode::SFPMuxIONode(const uhal::Node& node,
                           std::string uid_i2c_bus,
                           std::string pll_i2c_bus,
                           std::string pll_i2c_device,
                           std::vector<std::string> clock_names,
                           std::vector<std::string> sfp_i2c_buses)
  : IONode(node, uid_i2c_bus, pll_i2c_bus, pll_i2c_device, clock_names, sfp_i2c_buses)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SFPMuxIONode::~SFPMuxIONode() {}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq