#include <sstream>
#include <string>

namespace dunedaq::timing {

//-----------------------------------------------------------------------------
template<class IO, class MST>
MasterDesign<IO, MST>::MasterDesign(const uhal::Node& node)
  : TopDesign<IO>(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
MasterDesign<IO, MST>::~MasterDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
const MST&
MasterDesign<IO, MST>::get_master_node() const
{
  return uhal::Node::getNode<MST>("master");
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
uint64_t
MasterDesign<IO, MST>::read_master_timestamp() const
{
  return get_master_node().read_timestamp();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
void
MasterDesign<IO, MST>::sync_timestamp() const
{
  auto dts_clock_frequency = this->get_io_node().read_firmware_frequency();
  get_master_node().sync_timestamp(dts_clock_frequency);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t
MasterDesign<IO, MST>::measure_endpoint_rtt(uint32_t address, bool control_sfp) const
{
  return get_master_node().measure_endpoint_rtt(address, control_sfp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
void
MasterDesign<IO, MST>::apply_endpoint_delay(uint32_t address,
                                            uint32_t coarse_delay,
                                            uint32_t fine_delay,
                                            uint32_t phase_delay,
                                            bool measure_rtt,
                                            bool control_sfp) const
{
  get_master_node().apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay, measure_rtt, control_sfp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
void
MasterDesign<IO, MST>::send_fl_cmd(uint32_t command,                  // NOLINT(build/unsigned)
                                   uint32_t channel,                  // NOLINT(build/unsigned)
                                   uint32_t number_of_commands) const // NOLINT(build/unsigned)
{
    get_master_node().send_fl_cmd(command, channel, number_of_commands);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
void
MasterDesign<IO, MST>::enable_fake_trigger(uint32_t channel, double rate, bool poisson) const // NOLINT(build/unsigned)
{
  auto dts_clock_frequency = this->get_io_node().read_firmware_frequency();
  get_master_node().enable_fake_trigger(channel, rate, poisson, dts_clock_frequency);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t
MasterDesign<IO, MST>::read_firmware_version() const // NOLINT(build/unsigned)
{
  auto firmware_version = this->get_master_node().getNode("global.version").read();
  uhal::Node::getClient().dispatch();

  return firmware_version.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
void
MasterDesign<IO, MST>::validate_firmware_version() const
{
  auto firmware_version = read_firmware_version();

  int major_firmware_version = (firmware_version >> 16) & 0xff;
  int minor_firmware_version = (firmware_version >> 8) & 0xff;
  int patch_firmware_version = (firmware_version >> 0) & 0xff;

  if (major_firmware_version != g_required_major_master_firmware_version)
    throw IncompatibleMajorMasterFirmwareVersion(ERS_HERE, major_firmware_version, g_required_major_master_firmware_version);
  if (minor_firmware_version != g_required_minor_master_firmware_version)
    ers::warning(IncompatibleMinorMasterFirmwareVersion(ERS_HERE, minor_firmware_version, g_required_minor_master_firmware_version));
  if (patch_firmware_version != g_required_patch_master_firmware_version)
    ers::warning(IncompatiblePatchMasterFirmwareVersion(ERS_HERE, patch_firmware_version, g_required_patch_master_firmware_version));
}
//-----------------------------------------------------------------------------
}