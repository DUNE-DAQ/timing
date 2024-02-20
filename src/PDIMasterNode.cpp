/**
 * @file PDIMasterNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PDIMasterNode.hpp"

#include "timing/toolbox.hpp"
#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(PDIMasterNode)

//-----------------------------------------------------------------------------
PDIMasterNode::PDIMasterNode(const uhal::Node& node)
  : MasterNodeInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PDIMasterNode::~PDIMasterNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PDIMasterNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto raw_timestamp = getNode<TimestampGeneratorNode>("tstamp").read_raw_timestamp();
  status << "Timestamp: 0x" << std::hex << tstamp2int(raw_timestamp) << std::endl << std::endl;
  status << getNode<PDIFLCmdGeneratorNode>("scmd_gen").get_cmd_counters_table();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
PDIMasterNode::read_timestamp() const
{
  return getNode<TimestampGeneratorNode>("tstamp").read_timestamp();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::set_timestamp(uint64_t timestamp) const // NOLINT(build/unsigned)
{
  getNode<TimestampGeneratorNode>("tstamp").set_timestamp(timestamp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::sync_timestamp(uint32_t clock_frequency_hz) const // NOLINT(build/unsigned)
{
  const uint64_t old_timestamp = read_timestamp(); // NOLINT(build/unsigned)
  TLOG() << "Reading old timestamp: " << format_reg_value(old_timestamp) << ", " << format_timestamp(old_timestamp, clock_frequency_hz);

  const uint64_t now_timestamp = get_milliseconds_since_epoch() * clock_frequency_hz / 1000 ; // NOLINT(build/unsigned)
  TLOG() << "Setting new timestamp: " << format_reg_value(now_timestamp) << ", " << format_timestamp(now_timestamp, clock_frequency_hz);

  set_timestamp(now_timestamp);

  const uint64_t new_timestamp = read_timestamp(); // NOLINT(build/unsigned)
  TLOG() << "Reading new timestamp: " << format_reg_value(new_timestamp) << ", " << format_timestamp(new_timestamp, clock_frequency_hz);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PDIMasterNode::get_status_with_date(uint32_t clock_frequency_hz, bool print_out) const // NOLINT(build/unsigned)
{
  std::stringstream status;
  auto raw_timestamp = getNode<TimestampGeneratorNode>("tstamp").read_raw_timestamp();
  status << "Timestamp: 0x" << std::hex << tstamp2int(raw_timestamp) << " -> " << format_timestamp(raw_timestamp, clock_frequency_hz) << std::endl
          << std::endl;
  status << getNode<FLCmdGeneratorNode>("scmd_gen").get_cmd_counters_table();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::send_fl_cmd(FixedLengthCommandType command,
                           uint32_t channel,                  // NOLINT(build/unsigned)
                           uint32_t number_of_commands) const // NOLINT(build/unsigned)
{
  for (uint32_t i = 0; i < number_of_commands; i++) { // NOLINT(build/unsigned)
    getNode<PDIFLCmdGeneratorNode>("scmd_gen").send_fl_cmd(command, channel, getNode<TimestampGeneratorNode>("tstamp"));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::send_fl_cmd(uint32_t command,
                           uint32_t channel,                  // NOLINT(build/unsigned)
                           uint32_t number_of_commands) const // NOLINT(build/unsigned)
{
  send_fl_cmd(static_cast<FixedLengthCommandType>(command), channel, number_of_commands);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::switch_endpoint_sfp(uint32_t address, bool turn_on) const // NOLINT(build/unsigned)
{
  auto vl_cmd_node = getNode<VLCmdGeneratorNode>("acmd");

  // Switch off endpoint SFP tx
  vl_cmd_node.switch_endpoint_sfp(address, turn_on);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::enable_upstream_endpoint() const
{
  auto global = getNode<PDIMasterGlobalNode>("global");
  global.enable_upstream_endpoint();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t                                                                      // NOLINT(build/unsigned)
PDIMasterNode::measure_endpoint_rtt(uint32_t address, bool control_sfp) const // NOLINT(build/unsigned)
{

  auto vl_cmd_node = getNode<VLCmdGeneratorNode>("acmd");
  auto global = getNode<PDIMasterGlobalNode>("global");
  auto echo = getNode<EchoMonitorNode>("echo");

  if (control_sfp)
  {
    // Switch off all TX SFPs
    vl_cmd_node.switch_endpoint_sfp(0x0, false);

    // Turn on the current target
    vl_cmd_node.switch_endpoint_sfp(address, true);

    millisleep(200);
  }

  try
  {
    global.enable_upstream_endpoint();
  }
  catch (const timing::EndpointNotReady& e)
  {
    if (control_sfp) {
      vl_cmd_node.switch_endpoint_sfp(address, false);
    }
    throw e;
  }
  uint32_t endpoint_rtt = echo.send_echo_and_measure_delay(); // NOLINT(build/unsigned)

  if (control_sfp)
    vl_cmd_node.switch_endpoint_sfp(address, false);

  return endpoint_rtt;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::apply_endpoint_delay(uint32_t address,      // NOLINT(build/unsigned)
                                    uint32_t coarse_delay, // NOLINT(build/unsigned)
                                    uint32_t fine_delay,   // NOLINT(build/unsigned)
                                    uint32_t phase_delay,  // NOLINT(build/unsigned)
                                    bool measure_rtt,
                                    bool control_sfp) const
{

  auto vl_cmd_node = getNode<VLCmdGeneratorNode>("acmd");
  auto global = getNode<PDIMasterGlobalNode>("global");
  auto echo = getNode<EchoMonitorNode>("echo");
  
  if (measure_rtt) {
    uint64_t endpoint_rtt = measure_endpoint_rtt(address, control_sfp); // NOLINT(build/unsigned)
    TLOG() << "Pre delay adjustment RTT:  " << format_reg_value(endpoint_rtt,10);
  }

  vl_cmd_node.apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay);

  if (measure_rtt) {
    uint64_t endpoint_rtt = measure_endpoint_rtt(address, true); // NOLINT(build/unsigned)
    TLOG() << "Post delay adjustment RTT: " << format_reg_value(endpoint_rtt,10);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
const PartitionNode&
PDIMasterNode::get_partition_node(uint32_t partition_id) const // NOLINT(build/unsigned)
{
  const std::string node_name = "partition" + std::to_string(partition_id);
  return getNode<PartitionNode>(node_name);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::enable_spill_interface() const
{
  getNode<SpillInterfaceNode>("spill").enable();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::enable_fake_spills(uint32_t cycle_length, uint32_t spill_length) const // NOLINT(build/unsigned)
{
  getNode<SpillInterfaceNode>("spill").enable_fake_spills(cycle_length, spill_length);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
PDIMasterNode::read_in_spill() const
{
  return getNode<SpillInterfaceNode>("spill").read_in_spill();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::get_info(timingfirmwareinfo::PDIMasterMonitorData& mon_data) const
{
  auto timestamp = getNode<TimestampGeneratorNode>("tstamp").read_raw_timestamp();
  mon_data.timestamp = tstamp2int(timestamp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::get_info(opmonlib::InfoCollector& ic, int level) const
{
  timingfirmwareinfo::PDIMasterMonitorData mon_data;
  this->get_info(mon_data);
  ic.add(mon_data);

  for (int i=0; i < 4; ++i)
  {
    opmonlib::InfoCollector partition_ic;
    get_partition_node(i).get_info(partition_ic, level);
    ic.add("partition"+std::to_string(i),partition_ic);
  }

  getNode<FLCmdGeneratorNode>("scmd_gen").get_info(ic, level);

  getNode<SpillInterfaceNode>("spill").get_info(ic, level);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
timingfirmware::EndpointCheckResult
PDIMasterNode::scan_endpoint(uint16_t endpoint_address, bool control_sfp) const
{
  timingfirmware::EndpointCheckResult result;
  auto global = getNode<PDIMasterGlobalNode>("global");
  auto echo = getNode<EchoMonitorNode>("echo");

  timingfirmware::EndpointCheckResult endpoint_result;
  endpoint_result.address = endpoint_address;

  if (control_sfp)
  {
    switch_endpoint_sfp(endpoint_address, true);

    millisleep(100);
  }

  try
  {
    global.enable_upstream_endpoint();
  }
  catch (const timing::EndpointNotReady& e)
  {
    switch_endpoint_sfp(endpoint_address, false);

    TLOG_DEBUG(5) << "Endpoint at address " << endpoint_address << " looks dead.";

    return endpoint_result;
  }

  endpoint_result.alive = true;
  endpoint_result.round_trip_time = echo.send_echo_and_measure_delay();
  TLOG_DEBUG(5) << "Endpoint at address " << endpoint_address << " alive. RTT: " << endpoint_result.round_trip_time;

  if (control_sfp)
  {
    switch_endpoint_sfp(endpoint_address, false);
  }

  return endpoint_result;
}
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq
