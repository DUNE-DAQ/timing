/**
 * @file PDIFLCmdGeneratorNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PDIFLCmdGeneratorNode.hpp"

#include "timing/toolbox.hpp"
#include "logging/Logging.hpp"

#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(PDIFLCmdGeneratorNode)

//-----------------------------------------------------------------------------
PDIFLCmdGeneratorNode::PDIFLCmdGeneratorNode(const uhal::Node& node)
  : FLCmdGeneratorNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PDIFLCmdGeneratorNode::~PDIFLCmdGeneratorNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIFLCmdGeneratorNode::validate_channel(uint32_t channel) const // NOLINT(build/unsigned)
{
  if (channel > 0x4)
  {
    throw InvalidFixedLatencyCommandChannel(ERS_HERE, channel);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIFLCmdGeneratorNode::send_fl_cmd(FixedLengthCommandType command,
                                uint32_t channel) const // NOLINT(build/unsigned)
{
  validate_channel(channel);

  getNode("sel").write(channel);

  reset_sub_nodes(getNode("chan_ctrl"));

  getNode("chan_ctrl.type").write(command);
  getNode("chan_ctrl.force").write(0x1);
  getClient().dispatch();

  getNode("chan_ctrl.force").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PDIFLCmdGeneratorNode::send_fl_cmd(FixedLengthCommandType command,
                                uint32_t channel, // NOLINT(build/unsigned)
                                const TimestampGeneratorNode& timestamp_gen_node) const
{
  validate_channel(channel);

  getNode("sel").write(channel);

  reset_sub_nodes(getNode("chan_ctrl"));

  getNode("chan_ctrl.type").write(command);
  getNode("chan_ctrl.force").write(0x1);
  auto timestamp = timestamp_gen_node.read_raw_timestamp(false);

  getClient().dispatch();

  getNode("chan_ctrl.force").write(0x0);
  getClient().dispatch();
  TLOG() << "Command sent " << get_command_map().at(command) << "(" << format_reg_value(command) << ") from generator "
         << format_reg_value(channel) << " @time " << std::hex << std::showbase << tstamp2int(timestamp);
}
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq
