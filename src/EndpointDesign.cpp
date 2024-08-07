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
EndpointDesign::configure() const
{
  // Hard resets
  reset_io();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// void
// EndpointDesign::get_info(opmonlib::InfoCollector& ci, int level) const
// { 
//   opmonlib::InfoCollector endpoint_collector;
//   get_endpoint_node_plain(0)->get_info(endpoint_collector, level);
//   ci.add("endpoint", endpoint_collector);

//   opmonlib::InfoCollector hardware_collector;
//   get_io_node_plain()->get_info(hardware_collector, level);
//   ci.add("io", hardware_collector);
// }
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
} // namespace dunedaq::timing  
