/**
 * @file PDIHSIEndpointNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PDIHSIEndpointNode.hpp"

#include "timing/definitions.hpp"
#include "timing/toolbox.hpp"
#include "logging/Logging.hpp"

#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(PDIHSIEndpointNode)

//-----------------------------------------------------------------------------
PDIHSIEndpointNode::PDIHSIEndpointNode(const uhal::Node& node)
  : EndpointNodeInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PDIHSIEndpointNode::~PDIHSIEndpointNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIHSIEndpointNode::enable(uint32_t address, uint32_t partition) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.tgrp").write(partition);
  getNode("csr.ctrl.addr").write(address);
  getNode("csr.ctrl.ep_en").write(0x1);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIHSIEndpointNode::disable() const
{
  getNode("csr.ctrl.ep_en").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIHSIEndpointNode::reset(uint32_t address, uint32_t partition) const // NOLINT(build/unsigned)
{

  disable();
  enable(address, partition);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PDIHSIEndpointNode::get_status(bool print_out) const
{

  std::stringstream status;

  std::vector<std::pair<std::string, std::string>> ept_summary;

  auto ept_control = read_sub_nodes(getNode("csr.ctrl"), false);
  auto ept_state = read_sub_nodes(getNode("csr.stat"), false);
  getClient().dispatch();

  ept_summary.push_back(std::make_pair("State", g_endpoint_state_map.at(ept_state.find("ep_stat")->second.value())));
  ept_summary.push_back(std::make_pair("Partition", std::to_string(ept_control.find("tgrp")->second.value())));
  ept_summary.push_back(std::make_pair("Address", std::to_string(ept_control.find("addr")->second.value())));

  status << format_reg_table(ept_summary, "Endpoint summary", { "", "" }) << std::endl;
  status << format_reg_table(ept_state, "Endpoint state") << std::endl;

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
PDIHSIEndpointNode::get_info(timingendpointinfo::TimingEndpointInfo& mon_data) const
{
  auto endpoint_control = read_sub_nodes(getNode("csr.ctrl"), false);
  auto endpoint_state = read_sub_nodes(getNode("csr.stat"), false);
  getClient().dispatch();

  mon_data.state = endpoint_state.at("ep_stat").value();
  mon_data.ready = endpoint_state.at("ep_rdy").value();
  mon_data.partition = endpoint_control.at("tgrp").value();
  mon_data.address = endpoint_control.at("addr").value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIHSIEndpointNode::get_info(opmonlib::InfoCollector& ci, int /*level*/) const
{
  timingendpointinfo::TimingEndpointInfo mon_data;
  this->get_info(mon_data);
  ci.add(mon_data);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
