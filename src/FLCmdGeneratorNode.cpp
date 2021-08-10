/**
 * @file FLCmdGeneratorNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/FLCmdGeneratorNode.hpp"

#include "logging/Logging.hpp"

#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(FLCmdGeneratorNode)

//-----------------------------------------------------------------------------
FLCmdGeneratorNode::FLCmdGeneratorNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
FLCmdGeneratorNode::~FLCmdGeneratorNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FLCmdGeneratorNode::get_status(bool print_out) const
{
  std::stringstream lStatus;
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  lStatus << format_reg_table(subnodes, "FL Cmd gen state");
  if (print_out)
    TLOG() << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::send_fl_cmd(uint32_t command, // NOLINT(build/unsigned)
                                uint32_t channel, // NOLINT(build/unsigned)
                                const TimestampGeneratorNode& timestamp_gen_node) const
{
  getNode("sel").write(channel);

  reset_sub_nodes(getNode("chan_ctrl"));

  getNode("chan_ctrl.type").write(command);
  getNode("chan_ctrl.force").write(0x1);
  auto lTStamp = timestamp_gen_node.read_raw_timestamp(false);

  getClient().dispatch();

  getNode("chan_ctrl.force").write(0x0);
  getClient().dispatch();
  TLOG() << "Command sent " << g_command_map.at(command) << "(" << format_reg_value(command) << ") from generator "
         << format_reg_value(channel) << " @time 0x" << tstamp2int(lTStamp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::enable_fake_trigger(uint32_t channel,  // NOLINT(build/unsigned)
                                        uint32_t divisor,  // NOLINT(build/unsigned)
                                        uint32_t prescale, // NOLINT(build/unsigned)
                                        bool poisson) const
{
  uint32_t trigCmd = 0x8 + channel; // NOLINT(build/unsigned)

  getNode("sel").write(channel);

  getNode("chan_ctrl.type").write(trigCmd);
  getNode("chan_ctrl.rate_div_d").write(divisor);
  getNode("chan_ctrl.rate_div_p").write(prescale);
  getNode("chan_ctrl.patt").write(poisson);
  getNode("chan_ctrl.en").write(1); // Start the command stream
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::disable_fake_trigger(uint32_t channel) const // NOLINT(build/unsigned)
{
  // Clear the internal trigger generator.
  getNode("sel").write(channel);
  reset_sub_nodes(getNode("chan_ctrl"));
  TLOG() << "Fake trigger generator " << format_reg_value(channel) << " configuration cleared";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string
FLCmdGeneratorNode::get_cmd_counters_table(bool print_out) const
{
  std::stringstream lCountersTable;
  auto lAccCounters = getNode("actrs").readBlock(getNode("actrs").getSize());
  auto lRejCounters = getNode("rctrs").readBlock(getNode("actrs").getSize());
  getClient().dispatch();

  std::vector<uhal::ValVector<uint32_t>> lCountersContainer = { lAccCounters, lRejCounters }; // NOLINT(build/unsigned)

  lCountersTable << format_counters_table(lCountersContainer,
                                          { "Accept counters", "Reject counters" },
                                          "Cmd gen counters",
                                          { "0x0", "0x1", "0x2", "0x3", "0x4" },
                                          "Chan");

  if (print_out)
    TLOG() << lCountersTable.str();
  return lCountersTable.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::get_info(timingfirmwareinfo::TimingFLCmdChannelCounters& mon_data) const
{
  auto accepted_counters = getNode("actrs").readBlock(getNode("actrs").getSize());
  auto rejected_counters = getNode("rctrs").readBlock(getNode("actrs").getSize());
  getClient().dispatch();

  uint number_of_channels = 5;

  nlohmann::json cmd_data;
  
  for (uint i = 0; i < number_of_channels; ++i) { // NOLINT(build/unsigned)
    nlohmann::json cmd_datum;

    cmd_datum["accepted"] = accepted_counters.at(i);
    cmd_datum["rejected"] = rejected_counters.at(i);

    std::string channel = "channel_" + std::to_string(i);
    cmd_data[channel] = cmd_datum;
  }

  timingfirmwareinfo::from_json(cmd_data, mon_data);
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq