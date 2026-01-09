/**
 * @file MIBV3IONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/MIBV3IONode.hpp"

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(MIBV3IONode)

//-----------------------------------------------------------------------------
MIBV3IONode::MIBV3IONode(const uhal::Node& node)
  : IONode(node, "i2c", "i2c", { "PLL" }, { "OSC", "PLL", "EP 0", "EP 1", "EP 2" }, { "sfp0_i2c", "sfp1_i2c", "sfp2_i2c" })
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MIBV3IONode::~MIBV3IONode() {}
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq
