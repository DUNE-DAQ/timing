#include <sstream>
#include <string>

namespace dunedaq::timing {

//-----------------------------------------------------------------------------
template<class MST>
MasterDesign<MST>::MasterDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MasterDesignInterface(node)
  , TopDesign(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
MasterDesign<MST>::~MasterDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
std::string
MasterDesign<MST>::get_status(bool print_out) const
{
  std::stringstream status;
  status << this->get_io_node_plain()->get_pll_status();
  status << get_master_node().get_status();

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
void
MasterDesign<MST>::configure() const
{

  // Hard resets
  this->reset_io();

  // Set timestamp to current time
  this->sync_timestamp();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
const MST&
MasterDesign<MST>::get_master_node() const
{
  return uhal::Node::getNode<MST>("master");
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
uint64_t
MasterDesign<MST>::read_master_timestamp() const
{
  return get_master_node().read_timestamp();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
void
MasterDesign<MST>::sync_timestamp() const
{
  auto dts_clock_frequency = this->get_io_node_plain()->read_firmware_frequency();
  get_master_node().sync_timestamp(dts_clock_frequency);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
uint32_t
MasterDesign<MST>::measure_endpoint_rtt(uint32_t address, bool control_sfp, int /*sfp_mux*/) const
{
  return get_master_node().measure_endpoint_rtt(address, control_sfp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
void
MasterDesign<MST>::apply_endpoint_delay(uint32_t address,
                                            uint32_t coarse_delay,
                                            uint32_t fine_delay,
                                            uint32_t phase_delay,
                                            bool measure_rtt,
                                            bool control_sfp,
                                            int /*sfp_mux*/) const
{
  get_master_node().apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay, measure_rtt, control_sfp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
void
MasterDesign<MST>::send_fl_cmd(FixedLengthCommandType command,
                                   uint32_t channel,                  // NOLINT(build/unsigned)
                                   uint32_t number_of_commands) const // NOLINT(build/unsigned)
{
    get_master_node().send_fl_cmd(command, channel, number_of_commands);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
void
MasterDesign<MST>::enable_fake_trigger(uint32_t channel, double rate, bool poisson) const // NOLINT(build/unsigned)
{
  auto dts_clock_frequency = this->get_io_node_plain()->read_firmware_frequency();
  get_master_node().enable_fake_trigger(channel, rate, poisson, dts_clock_frequency);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
uint32_t
MasterDesign<MST>::read_firmware_version() const // NOLINT(build/unsigned)
{
  auto firmware_version = this->get_master_node().getNode("global.version").read();
  uhal::Node::getClient().dispatch();

  return firmware_version.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
void
MasterDesign<MST>::validate_firmware_version() const
{
  auto firmware_version = read_firmware_version();

  int major_firmware_version = (firmware_version >> 16) & 0xff;
  int minor_firmware_version = (firmware_version >> 8) & 0xff;
  int patch_firmware_version = (firmware_version >> 0) & 0xff;

  if (major_firmware_version != g_required_major_master_firmware_version)
    ers::error(IncompatibleMajorMasterFirmwareVersion(ERS_HERE, major_firmware_version, g_required_major_master_firmware_version));
  if (minor_firmware_version != g_required_minor_master_firmware_version)
    ers::warning(IncompatibleMinorMasterFirmwareVersion(ERS_HERE, minor_firmware_version, g_required_minor_master_firmware_version));
  if (patch_firmware_version != g_required_patch_master_firmware_version)
    ers::warning(IncompatiblePatchMasterFirmwareVersion(ERS_HERE, patch_firmware_version, g_required_patch_master_firmware_version));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
void
MasterDesign<MST>::get_info(opmonlib::InfoCollector& ci, int level) const
{ 
  opmonlib::InfoCollector master_collector;
  this->get_master_node().get_info(master_collector, level);
  ci.add("master", master_collector);

  opmonlib::InfoCollector hardware_collector;
  this->get_io_node_plain()->get_info(hardware_collector, level);
  ci.add("io", hardware_collector);
}
//-----------------------------------------------------------------------------
}
