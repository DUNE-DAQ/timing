/**
 * @file PartitionNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PartitionNode.hpp"

#include "logging/Logging.hpp"

#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(PartitionNode)

// Static data member initialization
const uint32_t PartitionNode::kWordsPerEvent = 6; // NOLINT(build/unsigned)

//-----------------------------------------------------------------------------
PartitionNode::PartitionNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PartitionNode::~PartitionNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::enable(bool enable, bool dispatch) const
{
  getNode("csr.ctrl.part_en").write(enable);

  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// void
// PartitionNode::writeTriggerMask( uint32_t aMask) const { // NOLINT(build/unsigned)
//     getNode("csr.ctrl.trig_mask").write(aMask);
//     getClient().dispatch();
// }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::configure(uint32_t trigger_mask, // NOLINT(build/unsigned)
                         bool enableSpillGate,
                         bool rate_control_enabled) const
{
  getNode("csr.ctrl.rate_ctrl_en").write(rate_control_enabled);
  getNode("csr.ctrl.trig_mask").write(trigger_mask);
  getNode("csr.ctrl.spill_gate_en").write(enableSpillGate);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::configure_rate_ctrl(bool rate_control_enabled) const
{
  getNode("csr.ctrl.rate_ctrl_en").write(rate_control_enabled);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::enable_triggers(bool enable) const
{
  // Disable the buffer
  getNode("csr.ctrl.trig_en").write(enable);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
PartitionNode::read_trigger_mask() const
{
  uhal::ValWord<uint32_t> lMask = getNode("csr.ctrl.trig_mask").read(); // NOLINT(build/unsigned)
  getClient().dispatch();

  return lMask;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
PartitionNode::read_buffer_word_count() const
{
  uhal::ValWord<uint32_t> lWords = getNode("buf.count").read(); // NOLINT(build/unsigned)
  getClient().dispatch();

  return lWords;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
PartitionNode::num_events_in_buffer() const
{
  return read_buffer_word_count() / kWordsPerEvent;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
PartitionNode::read_rob_warning_overflow() const
{
  uhal::ValWord<uint32_t> lWord = getNode("csr.stat.buf_warn").read(); // NOLINT(build/unsigned)
  getClient().dispatch();

  return lWord.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
PartitionNode::read_rob_error() const
{
  uhal::ValWord<uint32_t> lWord = getNode("csr.stat.buf_err").read(); // NOLINT(build/unsigned)
  getClient().dispatch();

  return lWord.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint32_t> // NOLINT(build/unsigned)
PartitionNode::read_events(size_t number_of_events) const
{

  uint32_t lEventsInBuffer = num_events_in_buffer(); // NOLINT(build/unsigned)

  uint32_t lEventsToRead = (number_of_events == 0 ? lEventsInBuffer : number_of_events); // NOLINT(build/unsigned)

  if (lEventsInBuffer < lEventsToRead) {
    throw EventReadError(ERS_HERE, number_of_events, lEventsInBuffer);
  }

  uhal::ValVector<uint32_t> lRawEvents = // NOLINT(build/unsigned)
    getNode("buf.data").readBlock(lEventsToRead * kWordsPerEvent);
  getClient().dispatch();

  return lRawEvents.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::reset() const
{
  // Disable partition
  getNode("csr.ctrl.part_en").write(0);

  // Disable buffer in partition 0
  getNode("csr.ctrl.buf_en").write(0);
  // Reset trigger counter
  getNode("csr.ctrl.trig_ctr_rst").write(1);
  // Release trigger counter
  getNode("csr.ctrl.trig_ctr_rst").write(0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::start(uint32_t timeout /*milliseconds*/) const // NOLINT(build/unsigned)
{

  // Disable triggers (just in case)
  getNode("csr.ctrl.trig_en").write(0);
  // Disable the buffer
  getNode("csr.ctrl.buf_en").write(0);
  getClient().dispatch();
  // Re-enable the buffer (flushes it)
  getNode("csr.ctrl.buf_en").write(1);
  getClient().dispatch();

  // Set the run bit and wait for it to be acknowledged
  getNode("csr.ctrl.run_req").write(1);
  getClient().dispatch();

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

  while (true) {
    auto lInRun = getNode("csr.stat.in_run").read();
    getClient().dispatch();

    if (lInRun)
      break;

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    if ((end - start) > std::chrono::milliseconds(timeout)) {
      throw RunRequestTimeoutExpired(ERS_HERE, timeout);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PartitionNode::stop(uint32_t timeout /*milliseconds*/) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.run_req").write(0);
  getClient().dispatch();

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

  while (true) {

    auto lInRun = getNode("csr.stat.in_run").read();
    getClient().dispatch();

    if (!lInRun)
      break;

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    if ((end - start) > std::chrono::milliseconds(timeout)) {
      throw RunRequestTimeoutExpired(ERS_HERE, timeout);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PartitionCounts
PartitionNode::read_command_counts() const
{

  const uhal::Node& lAccCtrs = getNode("actrs");
  const uhal::Node& lRejCtrs = getNode("rctrs");

  uhal::ValVector<uint32_t> lAccepted = lAccCtrs.readBlock(lAccCtrs.getSize()); // NOLINT(build/unsigned)
  uhal::ValVector<uint32_t> lRejected = lRejCtrs.readBlock(lRejCtrs.getSize()); // NOLINT(build/unsigned)
  getClient().dispatch();

  return { lAccepted.value(), lRejected.value() };
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PartitionNode::get_status(bool print_out) const
{
  std::stringstream lStatus;

  auto lControls = read_sub_nodes(getNode("csr.ctrl"), false);
  auto lState = read_sub_nodes(getNode("csr.stat"), false);

  auto lEventCtr = getNode("evtctr").read();
  auto lBufCount = getNode("buf.count").read();

  auto lAccCounters = getNode("actrs").readBlock(getNode("actrs").getSize());
  auto lRejCounters = getNode("rctrs").readBlock(getNode("actrs").getSize());

  getClient().dispatch();

  std::string lPartID = getId();
  std::string lPartNum = lPartID.substr(lPartID.find("partition") + 9);

  lStatus << "=> Partition " << lPartNum << std::endl;
  lStatus << std::endl;

  lStatus << format_reg_table(lControls, "Controls") << std::endl;
  lStatus << format_reg_table(lState, "State") << std::endl;

  lStatus << "Event Counter: " << lEventCtr.value() << std::endl;
  std::string lBufferStatusString = !lState.find("buf_err")->second.value() ? "OK" : "Error";
  lStatus << "Buffer status: " << lBufferStatusString << std::endl;
  lStatus << "Buffer occupancy: " << lBufCount.value() << std::endl;

  lStatus << std::endl;

  std::vector<uhal::ValVector<uint32_t>> lCountersContainer = { lAccCounters, lRejCounters }; // NOLINT(build/unsigned)

  lStatus << format_counters_table(lCountersContainer, { "Accept counters", "Reject counters" });

  if (print_out)
    TLOG() << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------
void
PartitionNode::get_info(timingfirmwareinfo::TimingPartitionMonitorData& mon_data) const
{
  auto lControls = read_sub_nodes(getNode("csr.ctrl"), false);
  auto lState = read_sub_nodes(getNode("csr.stat"), false);

  auto lEventCtr = getNode("evtctr").read();
  auto lBufCount = getNode("buf.count").read();

  auto accepted_counters = getNode("actrs").readBlock(getNode("actrs").getSize());
  auto rejected_counters = getNode("rctrs").readBlock(getNode("actrs").getSize());

  getClient().dispatch();

  mon_data.enabled = lControls.at("part_en").value();
  mon_data.spill_interface_enabled = lControls.at("spill_gate_en").value();
  mon_data.trig_enabled = lControls.at("trig_en").value();
  mon_data.trig_mask = lControls.at("trig_mask").value();
  mon_data.rate_ctrl_enabled = lControls.at("rate_ctrl_en").value();
  mon_data.frag_mask = lControls.at("frag_mask").value();
  mon_data.buffer_enabled = lControls.at("buf_en").value();

  mon_data.in_run = lState.at("in_run").value();
  mon_data.in_spill = lState.at("in_spill").value();

  mon_data.buffer_warning = lState.at("buf_warn").value();
  mon_data.buffer_error = lState.at("buf_err").value();
  mon_data.buffer_occupancy = lBufCount.value();

  nlohmann::json cmd_data;
  
  for (uint i = 0; i < g_command_map.size(); ++i) { // NOLINT(build/unsigned)
    nlohmann::json cmd_datum;

    cmd_datum["accepted"] = accepted_counters.at(i);
    cmd_datum["rejected"] = rejected_counters.at(i);

    cmd_data[g_command_map.at(i)] = cmd_datum;
  }

  timingfirmwareinfo::from_json(cmd_data, mon_data.fl_cmd_counters);
}
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq
