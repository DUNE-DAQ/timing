/**
 * @file FLCmdGeneratorNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/FLCmdGeneratorNode.hpp"

#include "timing/toolbox.hpp"
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
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "FL Cmd gen state");
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::validate_command(uint32_t command) const // NOLINT(build/unsigned)
{
  if (command > 0xff)
  {
    throw InvalidFixedLatencyCommand(ERS_HERE, command);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::validate_channel(uint32_t channel) const // NOLINT(build/unsigned)
{
  if (channel > 0x4)
  {
    throw InvalidFixedLatencyCommandChannel(ERS_HERE, channel);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::send_fl_cmd(uint32_t command,       // NOLINT(build/unsigned)
                                uint32_t channel) const // NOLINT(build/unsigned)
{
  validate_command(command);
  validate_channel(channel);

  getNode("sel").write(channel);

  reset_sub_nodes(getNode("chan_ctrl"));

  getNode("chan_ctrl.type").write(command);
  getNode("ctrl.force").write(0x1);
  getClient().dispatch();

  getNode("ctrl.force").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::enable_periodic_fl_cmd(uint32_t channel,  // NOLINT(build/unsigned)
                                           double rate,
                                           bool poisson,
                                           uint32_t clock_frequency_hz) const
{
  enable_periodic_fl_cmd(0x8+channel, channel, rate, poisson, clock_frequency_hz);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::enable_periodic_fl_cmd(uint32_t command,  // NOLINT(build/unsigned)
                                           uint32_t channel,  // NOLINT(build/unsigned)
                                           double rate,
                                           bool poisson,
                                           uint32_t clock_frequency_hz) const
{
  uint32_t divisor;
  uint32_t prescale;
  double actual_rate;

  parse_periodic_fl_cmd_rate(rate, clock_frequency_hz, actual_rate, divisor, prescale);

  TLOG() << "Requested rate, actual rate: " << rate << ", " << actual_rate;
  TLOG() << "prescale, divisor: " << prescale << ", " << divisor;

  std::stringstream trig_stream;
  trig_stream << "> Periodic rate for command 0x" << std::hex << command << ", on channel 0x" << channel
              << " set to " << std::setprecision(3) << std::scientific << actual_rate << " Hz";
  TLOG() << trig_stream.str();

  std::stringstream trigger_mode_stream;
  trigger_mode_stream << "> Trigger mode: ";

  if (poisson) {
    trigger_mode_stream << "poisson";
  } else {
    trigger_mode_stream << "periodic";
  }
  TLOG() << trigger_mode_stream.str();
  enable_periodic_fl_cmd(command, channel, divisor, prescale, poisson);

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::enable_periodic_fl_cmd(uint32_t channel,  // NOLINT(build/unsigned)
                                        uint32_t divisor,  // NOLINT(build/unsigned)
                                        uint32_t prescale, // NOLINT(build/unsigned)
                                        bool poisson) const
{
  enable_periodic_fl_cmd(0x8+channel, channel, divisor, prescale, poisson);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FLCmdGeneratorNode::enable_periodic_fl_cmd(uint32_t command,  // NOLINT(build/unsigned)
                                        uint32_t channel,  // NOLINT(build/unsigned)
                                        uint32_t divisor,  // NOLINT(build/unsigned)
                                        uint32_t prescale, // NOLINT(build/unsigned)
                                        bool poisson) const
{
  validate_command(command);
  validate_channel(channel);

  getNode("sel").write(channel);

  getNode("chan_ctrl.type").write(command);
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
  validate_channel(channel);

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
  std::stringstream counters_table;
  auto accepted_counters = getNode("actrs").readBlock(getNode("actrs").getSize());
  auto rejected_counters = getNode("rctrs").readBlock(getNode("actrs").getSize());
  getClient().dispatch();

  std::vector<uhal::ValVector<uint32_t>> counters_container = { accepted_counters, rejected_counters }; // NOLINT(build/unsigned)

  counters_table << format_counters_table(counters_container,
                                          { "Accept counters", "Reject counters" },
                                          "Cmd gen counters",
                                          { "0x0", "0x1", "0x2", "0x3", "0x4" },
                                          "Chan");

  if (print_out)
    TLOG() << counters_table.str();
  return counters_table.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// void
// FLCmdGeneratorNode::get_info(opmonlib::InfoCollector& ic, int /*level*/) const
// {
//   auto accepted_counters = getNode("actrs").readBlock(getNode("actrs").getSize());
//   auto rejected_counters = getNode("rctrs").readBlock(getNode("actrs").getSize());
//   getClient().dispatch();

//   uint number_of_channels = 5;

//   for (uint i = 0; i < number_of_channels; ++i) { // NOLINT(build/unsigned)

//     timingfirmwareinfo::TimingFLCmdCounter cmd_counter;
//     opmonlib::InfoCollector cmd_counter_ic;

//     cmd_counter.accepted = accepted_counters.at(i);
//     cmd_counter.rejected = rejected_counters.at(i);

//     std::string channel = "fl_cmd_channel_" + std::to_string(i);

//     cmd_counter_ic.add(cmd_counter);
//     ic.add(channel, cmd_counter_ic);
//   }
// }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void FLCmdGeneratorNode::parse_periodic_fl_cmd_rate(double requested_rate,
                                                    uint32_t clock_frequency_hz,
                                                    double& actual_rate,
                                                    uint32_t& divisor,
                                                    uint32_t& prescale)
{
  // Rate =  (clock_frequency_hz / 2^(d+8)) / p where n in [0,15] and p in [1,256]

  // DIVIDER (int): Frequency divider.

  // The division from clock_frequency_hz to the desired rate is done in three steps:
  // a) A pre-division by 256
  // b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
  // c) 1-in-n prescaling set by n = rate_div_p

  double div = ceil(log(clock_frequency_hz / (requested_rate * 256 * 256)) / log(2));
  if (div < 0) {
    divisor = 0;
  } else if (div > 15) {
    divisor = 15;
  } else {
    divisor = div;
  }

  uint32_t ps = (uint32_t)((clock_frequency_hz / (requested_rate * 256 * (1 << divisor))) + 0.5); // NOLINT(build/unsigned)
  if (ps == 0 || ps > 256)
  {
    throw BadRequestedFakeTriggerRate(ERS_HERE, requested_rate, ps);
  } else {
    prescale = ps;
  }
  actual_rate = static_cast<double>(clock_frequency_hz) / (256 * prescale * (1 << divisor));
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
