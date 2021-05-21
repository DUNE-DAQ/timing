/**
 * @file EndpointNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/EndpointNode.hpp"

#include "logging/Logging.hpp"

#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(EndpointNode)

//-----------------------------------------------------------------------------
EndpointNode::EndpointNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
EndpointNode::~EndpointNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
EndpointNode::enable(uint32_t partition, uint32_t address) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.tgrp").write(partition);

  if (address) {
    getNode("csr.ctrl.int_addr").write(0x1);
    getNode("csr.ctrl.addr").write(address);
  } else {
    getNode("csr.ctrl.int_addr").write(0x0);
  }

  getNode("csr.ctrl.ctr_rst").write(0x1);
  getNode("csr.ctrl.ctr_rst").write(0x0);
  getNode("csr.ctrl.ep_en").write(0x1);
  getNode("csr.ctrl.buf_en").write(0x1);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
EndpointNode::disable() const
{
  getNode("csr.ctrl.ep_en").write(0x0);
  getNode("csr.ctrl.buf_en").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
EndpointNode::reset(uint32_t partition, uint32_t address) const // NOLINT(build/unsigned)
{

  getNode("csr.ctrl.ep_en").write(0x0);
  getNode("csr.ctrl.buf_en").write(0x0);

  enable(partition, address);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
EndpointNode::get_status(bool print_out) const
{

  std::stringstream lStatus;

  std::vector<std::pair<std::string, std::string>> lEPSummary;

  auto lEPTimestamp = getNode("tstamp").readBlock(2);
  auto lEPEventCounter = getNode("evtctr").read();
  auto lEPBufferCount = getNode("buf.count").read();
  auto lEPControl = read_sub_nodes(getNode("csr.ctrl"), false);
  auto lEPState = read_sub_nodes(getNode("csr.stat"), false);
  auto lEPCounters = getNode("ctrs").readBlock(g_command_number);
  getClient().dispatch();

  lEPSummary.push_back(std::make_pair("State", g_endpoint_state_map.at(lEPState.find("ep_stat")->second.value())));
  lEPSummary.push_back(std::make_pair("Partition", std::to_string(lEPControl.find("tgrp")->second.value())));
  lEPSummary.push_back(std::make_pair("Address", std::to_string(lEPControl.find("addr")->second.value())));
  lEPSummary.push_back(std::make_pair("Timestamp", format_timestamp(lEPTimestamp)));
  lEPSummary.push_back(std::make_pair("Timestamp (hex)", format_reg_value(tstamp2int(lEPTimestamp))));
  lEPSummary.push_back(std::make_pair("EventCounter", std::to_string(lEPEventCounter.value())));
  std::string lBufferStatusString = !lEPState.find("buf_err")->second.value() ? "OK" : "Error";
  lEPSummary.push_back(std::make_pair("Buffer status", lBufferStatusString));
  lEPSummary.push_back(std::make_pair("Buffer occupancy", std::to_string(lEPBufferCount.value())));

  std::vector<std::pair<std::string, std::string>> lEPCommandCounters;

  for (uint32_t i = 0; i < g_command_number; ++i) { // NOLINT(build/unsigned)
    lEPCommandCounters.push_back(std::make_pair(g_command_map.at(i), std::to_string(lEPCounters[i])));
  }

  lStatus << format_reg_table(lEPSummary, "Endpoint summary", { "", "" }) << std::endl;
  lStatus << format_reg_table(lEPState, "Endpoint state") << std::endl;
  lStatus << format_reg_table(lEPCommandCounters, "Endpoint counters", { "Command", "Counter" });

  if (print_out)
    TLOG() << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
EndpointNode::read_timestamp() const
{
  auto lTimestamp = getNode("tstamp").readBlock(2);
  getClient().dispatch();
  return tstamp2int(lTimestamp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
EndpointNode::read_buffer_count() const
{
  auto lBufCount = getNode("buf.count").read();
  getClient().dispatch();
  return lBufCount.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uhal::ValVector<uint32_t> // NOLINT(build/unsigned)
EndpointNode::read_data_buffer(bool read_all) const
{

  auto lBufCount = getNode("buf.count").read();
  getClient().dispatch();

  TLOG_DEBUG(0) << "Words available in readout buffer:      " << format_reg_value(lBufCount);

  uint32_t lEventsToRead = lBufCount.value() / g_event_size; // NOLINT(build/unsigned)

  TLOG_DEBUG(0) << "Events available in readout buffer:     " << format_reg_value(lEventsToRead);

  uint32_t lWordsToRead = read_all ? lBufCount.value() : lEventsToRead * g_event_size; // NOLINT(build/unsigned)

  TLOG_DEBUG(0) << "Words to be read out in readout buffer: " << format_reg_value(lWordsToRead);

  if (!lWordsToRead) {
    TLOG() << "No words to be read out.";
  }

  auto lBufData = getNode("buf.data").readBlock(lWordsToRead);
  getClient().dispatch();

  return lBufData;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
EndpointNode::get_data_buffer_table(bool read_all, bool print_out) const
{

  std::stringstream lTable;
  auto lBufData = read_data_buffer(read_all);

  std::vector<std::pair<std::string, uint32_t>> lBufferTable; // NOLINT(build/unsigned)

  uint32_t i = 0; // NOLINT(build/unsigned)
  for (auto it = lBufData.begin(); it != lBufData.end(); ++it, ++i) {
    std::stringstream lIndexStream;
    lIndexStream << std::setfill('0') << std::setw(4) << i;
    lBufferTable.push_back(std::make_pair(lIndexStream.str(), *it));
  }
  lTable << format_reg_table(lBufferTable, "Endpoint buffer", { "Word", "Data" });

  if (print_out)
    TLOG() << lTable.str();
  return lTable.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
EndpointNode::read_clock_frequency() const
{
  std::vector<double> lFrequencies = getNode<FrequencyCounterNode>("freq").measure_frequencies(1);
  return lFrequencies.at(0);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
EndpointNode::read_version() const
{
  auto lBufCount = getNode("version").read();
  getClient().dispatch();
  return lBufCount.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
EndpointNode::get_info(timingendpointinfo::TimingEndpointInfo& mon_data) const
{

  auto timestamp = getNode("tstamp").readBlock(2);
  auto event_counter = getNode("evtctr").read();
  auto buffer_count = getNode("buf.count").read();
  auto endpoint_control = read_sub_nodes(getNode("csr.ctrl"), false);
  auto endpoint_state = read_sub_nodes(getNode("csr.stat"), false);
  auto counters = getNode("ctrs").readBlock(g_command_number);
  getClient().dispatch();

  mon_data.state = endpoint_state.at("ep_stat").value();
  mon_data.ready = endpoint_state.at("ep_rdy").value();
  mon_data.partition = endpoint_control.at("tgrp").value();
  mon_data.address = endpoint_control.at("addr").value();
  mon_data.timestamp = tstamp2int(timestamp);
  mon_data.in_run = endpoint_state.at("in_run").value();
  mon_data.in_spill = endpoint_state.at("in_spill").value();
  mon_data.buffer_warning = endpoint_state.at("buf_warn").value();
  mon_data.buffer_error = endpoint_state.at("buf_err").value();
  mon_data.buffer_occupancy = buffer_count.value();
  mon_data.event_counter = event_counter.value();
  mon_data.reset_out = endpoint_state.at("ep_rsto").value();
  mon_data.sfp_tx_disable = endpoint_state.at("sfp_tx_dis").value();
  mon_data.coarse_delay = endpoint_state.at("cdelay").value();
  mon_data.fine_delay = endpoint_state.at("fdelay").value();

  // for (uint32_t i=0; i < g_command_number; ++i) {
  //	counters.push_back(std::make_pair(g_command_map.at(i), std::to_string(counters[i])));
  //}
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq