/**
 * @file MasterNodeInterface.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/MasterNodeInterface.hpp"

#include <string>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
MasterNodeInterface::MasterNodeInterface(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MasterNodeInterface::~MasterNodeInterface() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterNodeInterface::apply_endpoint_delay(const ActiveEndpointConfig& ept_config, bool measure_rtt) const
{
  std::string lEptIdD = ept_config.id;
  uint32_t ept_address = ept_config.adr;    // NOLINT(build/unsigned)
  uint32_t coarse_Delay = ept_config.cdelay; // NOLINT(build/unsigned)
  uint32_t fine_delay = ept_config.fdelay; // NOLINT(build/unsigned)
  uint32_t phase_delay = ept_config.pdelay; // NOLINT(build/unsigned)
  apply_endpoint_delay(ept_address, coarse_Delay, fine_delay, phase_delay, measure_rtt);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterNodeInterface::enable_periodic_fl_cmd(uint32_t channel, double rate, bool poisson, uint32_t clock_frequency_hz) const // NOLINT(build/unsigned)
{
  enable_periodic_fl_cmd(0x8+channel, channel, rate, poisson, clock_frequency_hz);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterNodeInterface::enable_periodic_fl_cmd(uint32_t command, uint32_t channel, double rate, bool poisson, uint32_t clock_frequency_hz) const // NOLINT(build/unsigned)
{

  getNode<FLCmdGeneratorNode>("scmd_gen").enable_periodic_fl_cmd(command, channel, rate, poisson, clock_frequency_hz);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterNodeInterface::disable_periodic_fl_cmd(uint32_t channel) const // NOLINT(build/unsigned)
{
  getNode<FLCmdGeneratorNode>("scmd_gen").disable_fake_trigger(channel);
}
//------------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq