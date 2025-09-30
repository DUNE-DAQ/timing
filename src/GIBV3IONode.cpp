/**
 * @file GIBV3IONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/GIBV3IONode.hpp"

#include <string>
#include <math.h>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(GIBV3IONode)

//-----------------------------------------------------------------------------
GIBV3IONode::GIBV3IONode(const uhal::Node& node)
  : GIBV2IONode(node)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
GIBV3IONode::~GIBV3IONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
GIBV3IONode::get_status(bool print_out) const
{
  std::stringstream status;

  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "GIB IO state");

  auto subnodes_2 = read_sub_nodes(getNode("csr.ctrl"));
  status << format_reg_table(subnodes_2, "GIB IO control");

  // removed temperature readout

  if (print_out)
    TLOG() << std::endl << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
