/**
 * @file TimingNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/TimingNode.hpp"

#include <map>
#include <string>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
TimingNode::TimingNode(const uhal::Node& node)
  : uhal::Node(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TimingNode::~TimingNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::map<std::string, uhal::ValWord<uint32_t>> // NOLINT(build/unsigned)
TimingNode::read_sub_nodes(const uhal::Node& node, bool dispatch) const
{
  auto node_names = node.getNodes();

  std::map<std::string, uhal::ValWord<uint32_t>> node_name_value_pairs; // NOLINT(build/unsigned)

  for (auto it = node_names.begin(); it != node_names.end(); ++it)
    node_name_value_pairs[*it] = node.getNode(*it).read();
  if (dispatch)
    getClient().dispatch();
  return node_name_value_pairs;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
TimingNode::reset_sub_nodes(const uhal::Node& node, uint32_t aValue, bool dispatch) const // NOLINT(build/unsigned)
{

  auto node_names = node.getNodes();

  for (auto it = node_names.begin(); it != node_names.end(); ++it)
    node.getNode(*it).write(aValue);

  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq