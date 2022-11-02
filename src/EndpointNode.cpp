/**
 * @file EndpointNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/EndpointNode.hpp"
#include "timing/toolbox.hpp"

#include "logging/Logging.hpp"

#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(EndpointNode)

//-----------------------------------------------------------------------------
EndpointNode::EndpointNode(const uhal::Node& node)
  : EndpointNodeInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
EndpointNode::~EndpointNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
EndpointNode::enable(uint32_t address, uint32_t /*partition*/) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.addr").write(address);

  getNode("csr.ctrl.ep_en").write(0x1);
  getClient().dispatch();

  auto start = std::chrono::high_resolution_clock::now();

  std::chrono::milliseconds ms_since_start(0);

  uhal::ValWord<uint32_t> counters_ready;

  // Wait for the endpoint to be happy
  while (ms_since_start.count() < 500) {
    auto now = std::chrono::high_resolution_clock::now();
    ms_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

    millisleep(10);

    counters_ready = getNode("csr.stat.ctrs_rdy").read();
    getClient().dispatch();
    TLOG_DEBUG(1) << "counters_ready: 0x" << std::hex << counters_ready.value();

    if (counters_ready) {
      TLOG_DEBUG(1) << "counters ready";
      break;
    }
  }

  if (!counters_ready) {
    TLOG() << "counters failed to clear";
    // TODO throw something
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
EndpointNode::disable() const
{
  getNode("csr.ctrl.ep_en").write(0x0);
//  getNode("csr.ctrl.buf_en").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
EndpointNode::reset(uint32_t address, uint32_t /*partition*/) const // NOLINT(build/unsigned)
{

  getNode("csr.ctrl.ep_en").write(0x0);
  //getNode("csr.ctrl.buf_en").write(0x0);

  enable(address);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
EndpointNode::get_status(bool print_out) const
{

  std::stringstream status;

  std::vector<std::pair<std::string, std::string>> ept_summary;

  auto ept_timestamp = getNode("tstamp").readBlock(2);
  auto ept_control = read_sub_nodes(getNode("csr.ctrl"), false);
  auto ept_state = read_sub_nodes(getNode("csr.stat"), false);
  getNode("cmd_ctrs.addr").write(0x0);
  auto counters = getNode("cmd_ctrs.data").readBlock(0x14);
  getClient().dispatch();

  ept_summary.push_back(std::make_pair("Enabled", std::to_string(ept_control.find("ep_en")->second.value())));
  ept_summary.push_back(std::make_pair("Address", std::to_string(ept_control.find("addr")->second.value())));

  ept_summary.push_back(std::make_pair("Timestamp (hex)", format_reg_value(tstamp2int(ept_timestamp))));
  ept_summary.push_back(std::make_pair("Timestamp", format_timestamp(ept_timestamp,62500000)));

  status << format_reg_table(ept_summary, "Endpoint summary", { "", "" }) << std::endl;
//  status << "Endpoint frequency: " << ept_clock_frequency << " MHz" << std::endl;
  status << format_reg_table(ept_state, "Endpoint state") << std::endl;
  
  std::vector<uhal::ValVector<uint32_t>> counters_container = { counters }; // NOLINT(build/unsigned)

  std::vector<std::string> counter_labels;
  for (uint i=0; i < counters.size(); ++i) 
  {
    counter_labels.push_back(format_reg_value(i));
  }
  status << format_counters_table(counters_container, { "Received cmd counters" }, "Endpoint cmd counters", counter_labels);
  status << std::endl;

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
EndpointNode::read_timestamp() const
{
  auto timestamp = getNode("tstamp").readBlock(2);
  getClient().dispatch();
  return tstamp2int(timestamp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
EndpointNode::get_info(timingendpointinfo::TimingEndpointInfo& mon_data) const
{
  auto timestamp = getNode("tstamp").readBlock(2);
  auto endpoint_control = read_sub_nodes(getNode("csr.ctrl"), false);
  auto endpoint_state = read_sub_nodes(getNode("csr.stat"), false);
  getClient().dispatch();

  mon_data.state = endpoint_state.at("ep_stat").value();
  mon_data.ready = endpoint_state.at("ep_rdy").value();
  mon_data.address = endpoint_control.at("addr").value();
  mon_data.timestamp = tstamp2int(timestamp);
  mon_data.sfp_tx_disable = !endpoint_state.at("ep_txen").value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
EndpointNode::get_info(opmonlib::InfoCollector& ci, int /*level*/) const
{
  timingendpointinfo::TimingEndpointInfo mon_data;
  this->get_info(mon_data);
  ci.add(mon_data);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
