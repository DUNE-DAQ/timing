/**
 * @file EndpointDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/EndpointDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(EndpointDesign)

//-----------------------------------------------------------------------------
EndpointDesign::EndpointDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , TopDesign(node)
  , EndpointDesignInterface(node)

{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
EndpointDesign::~EndpointDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
EndpointDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << TopDesign::get_io_node_plain()->get_pll_status();
  size_t number_of_endpoint_nodes = EndpointDesign::get_number_of_endpoint_nodes(); 
  for (size_t i = 0; i < number_of_endpoint_nodes; ++i) {
    status << "Endpoint node " << i << " status" << std::endl;
    status << get_endpoint_node_plain(i)->get_status();
  }
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
EndpointDesign::configure(ClockSource clock_source) const
{
  TopDesign::configure(clock_source);

  for (uint i=0; i <  get_number_of_endpoint_nodes(); ++i)
  {
    get_endpoint_node_plain(i)->reset(0x30+i);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    get_endpoint_node_plain(i)->get_status(true);
    if (!get_endpoint_node_plain(i)->endpoint_ready())
    {
      ers::error(EndpointNotReady(ERS_HERE, "Endpoint "+std::to_string(i)+" not ready!", get_endpoint_node_plain(i)->read_endpoint_state()));
    }
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
EndpointDesign::read_firmware_version() const
{
  return 0; //TODO
  //return get_endpoint_node_plain(0)->read_version();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
EndpointDesign::validate_firmware_version() const
{
// TODO
//  auto firmware_version = read_firmware_version();
//  
//  int major_firmware_version = (firmware_version >> 16) & 0xff;
//  int minor_firmware_version = (firmware_version >> 8) & 0xff;
//  int patch_firmware_version = (firmware_version >> 0) & 0xff;
//
//  if (major_firmware_version != g_required_major_endpoint_firmware_version)
//    ers::error(IncompatibleMajorEndpointFirmwareVersion(ERS_HERE, major_firmware_version, g_required_major_endpoint_firmware_version));
//  if (minor_firmware_version != g_required_minor_endpoint_firmware_version)
//    ers::warning(IncompatibleMinorEndpointFirmwareVersion(ERS_HERE, minor_firmware_version, g_required_minor_endpoint_firmware_version));
//  if (patch_firmware_version != g_required_patch_endpoint_firmware_version)
//    ers::warning(IncompatiblePatchEndpointFirmwareVersion(ERS_HERE, patch_firmware_version, g_required_patch_endpoint_firmware_version));
}
//-----------------------------------------------------------------------------

void
EndpointDesign::get_info(timingfirmwareinfo::TimingDeviceInfo& mon_data) const
{
  TopDesign::get_info(mon_data);
  EndpointDesignInterface::get_info(0, mon_data.endpoint_info);
}
} // namespace dunedaq::timing  
