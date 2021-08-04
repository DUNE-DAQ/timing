/**
 * @file PDIMasterNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PDIMasterNode.hpp"

#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(PDIMasterNode)

//-----------------------------------------------------------------------------
PDIMasterNode::PDIMasterNode(const uhal::Node& node)
  : MasterNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PDIMasterNode::~PDIMasterNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PDIMasterNode::get_status(bool print_out) const
{
  std::stringstream lStatus;
  auto lTStamp = getNode<TimestampGeneratorNode>("tstamp").read_raw_timestamp();
  lStatus << "Timestamp: 0x" << std::hex << tstamp2int(lTStamp) << " -> " << format_timestamp(lTStamp) << std::endl
          << std::endl;
  lStatus << getNode<FLCmdGeneratorNode>("scmd_gen").get_cmd_counters_table();
  if (print_out)
    TLOG() << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::switch_endpoint_sfp(uint32_t address, bool turn_on) const // NOLINT(build/unsigned)
{
  auto vlCmdNode = getNode<VLCmdGeneratorNode>("acmd");

  // Switch off endpoint SFP tx
  vlCmdNode.switch_endpoint_sfp(address, turn_on);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::enable_upstream_endpoint() const
{
  auto lGlobal = getNode<GlobalNode>("global");
  lGlobal.enable_upstream_endpoint();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t                                                                      // NOLINT(build/unsigned)
PDIMasterNode::measure_endpoint_rtt(uint32_t address, bool control_sfp) const // NOLINT(build/unsigned)
{

  auto vlCmdNode = getNode<VLCmdGeneratorNode>("acmd");
  auto lGlobal = getNode<GlobalNode>("global");
  auto lEcho = getNode<EchoMonitorNode>("echo");

  if (control_sfp) {
    // Switch off all TX SFPs
    vlCmdNode.switch_endpoint_sfp(0x0, false);

    // Turn on the current target
    vlCmdNode.switch_endpoint_sfp(address, true);

    millisleep(100);
  }

  lGlobal.enable_upstream_endpoint();

  uint32_t lEndpointRTT = lEcho.send_echo_and_measure_delay(); // NOLINT(build/unsigned)

  if (control_sfp)
    vlCmdNode.switch_endpoint_sfp(address, false);

  return lEndpointRTT;
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

  auto vlCmdNode = getNode<VLCmdGeneratorNode>("acmd");
  auto lGlobal = getNode<GlobalNode>("global");
  auto lEcho = getNode<EchoMonitorNode>("echo");

  if (measure_rtt) {

    if (control_sfp) {
      // Switch off all TX SFPs
      vlCmdNode.switch_endpoint_sfp(0x0, false);

      // Turn on the current target
      vlCmdNode.switch_endpoint_sfp(address, true);

      millisleep(100);
    }

    lGlobal.enable_upstream_endpoint();

    uint64_t lEndpointRTT = lEcho.send_echo_and_measure_delay(); // NOLINT(build/unsigned)
    TLOG() << "Pre delay adjustment RTT:  " << format_reg_value(lEndpointRTT);
  }

  vlCmdNode.apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay);

  if (measure_rtt) {
    millisleep(100);

    lGlobal.enable_upstream_endpoint();

    uint64_t lEndpointRTT = lEcho.send_echo_and_measure_delay(); // NOLINT(build/unsigned)
    TLOG() << "Post delay adjustment RTT: " << format_reg_value(lEndpointRTT);

    if (control_sfp)
      vlCmdNode.switch_endpoint_sfp(address, false);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::send_fl_cmd(uint32_t command,                  // NOLINT(build/unsigned)
                           uint32_t channel,                  // NOLINT(build/unsigned)
                           uint32_t number_of_commands) const // NOLINT(build/unsigned)
{
  for (uint32_t i = 0; i < number_of_commands; i++) { // NOLINT(build/unsigned)
    getNode<FLCmdGeneratorNode>("scmd_gen").send_fl_cmd(command, channel, getNode<TimestampGeneratorNode>("tstamp"));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::enable_fake_trigger(uint32_t channel, double rate, bool poisson) const // NOLINT(build/unsigned)
{

  // Configures the internal command generator to produce triggers at a defined frequency.
  // Rate =  (50MHz / 2^(d+8)) / p where n in [0,15] and p in [1,256]

  // DIVIDER (int): Frequency divider.

  // The division from 50MHz to the desired rate is done in three steps:
  // a) A pre-division by 256
  // b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
  // c) 1-in-n prescaling set by n = rate_div_p

  FakeTriggerConfig lFTConfig(rate);

  lFTConfig.print();
  std::stringstream trig_stream;
  trig_stream << "> Trigger rate for FakeTrig" << channel << " (" << std::showbase << std::hex << 0x8 + channel
              << ") set to " << std::setprecision(3) << std::scientific << lFTConfig.actual_rate << " Hz";
  TLOG() << trig_stream.str();

  std::stringstream lTriggerModeStream;
  lTriggerModeStream << "> Trigger mode: ";

  if (poisson) {
    lTriggerModeStream << "poisson";
  } else {
    lTriggerModeStream << "periodic";
  }
  TLOG() << lTriggerModeStream.str();
  getNode<FLCmdGeneratorNode>("scmd_gen").enable_fake_trigger(channel, lFTConfig.divisor, lFTConfig.prescale, poisson);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::disable_fake_trigger(uint32_t channel) const // NOLINT(build/unsigned)
{
  getNode<FLCmdGeneratorNode>("scmd_gen").disable_fake_trigger(channel);
}
//------------------------------------------------------------------------------

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
PDIMasterNode::sync_timestamp() const
{

  const uint64_t lOldTimestamp = read_timestamp(); // NOLINT(build/unsigned)
  TLOG() << "Old timestamp: " << format_reg_value(lOldTimestamp) << ", " << format_timestamp(lOldTimestamp);

  const uint64_t lNow = get_seconds_since_epoch() * g_dune_sp_clock_in_hz; // NOLINT(build/unsigned)
  set_timestamp(lNow);

  const uint64_t lNewTimestamp = read_timestamp(); // NOLINT(build/unsigned)
  TLOG() << "New timestamp: " << format_reg_value(lNewTimestamp) << ", " << format_timestamp(lNewTimestamp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::get_info(timingfirmwareinfo::PDIMasterMonitorData& mon_data) const
{
  auto lTStamp = getNode<TimestampGeneratorNode>("tstamp").read_raw_timestamp();
  mon_data.timestamp = tstamp2int(lTStamp);

  auto spill_interface_enabled = getNode("spill.csr.ctrl.en").read();
  getClient().dispatch();

  mon_data.spill_interface_enabled = spill_interface_enabled.value();

  get_partition_node(0).get_info(mon_data.partition_0);
  get_partition_node(1).get_info(mon_data.partition_1);
  get_partition_node(2).get_info(mon_data.partition_2);
  get_partition_node(3).get_info(mon_data.partition_3);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIMasterNode::get_info(timingfirmwareinfo::PDIMasterMonitorDataDebug& mon_data) const
{
  getNode<FLCmdGeneratorNode>("scmd_gen").get_info(mon_data.sent_fl_cmd_chan_counters);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
