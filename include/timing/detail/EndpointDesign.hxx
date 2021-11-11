#include <sstream>
#include <string>

namespace dunedaq::timing {

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node*
EndpointDesign<IO>::clone() const
{
  return new EndpointDesign<IO>(static_cast<const EndpointDesign<IO>&>(*this));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
EndpointDesign<IO>::EndpointDesign(const uhal::Node& node)
  : TopDesign<IO>(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
EndpointDesign<IO>::~EndpointDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
std::string
EndpointDesign<IO>::get_status(bool print_out) const
{
  std::stringstream status;
  status << this->get_io_node().get_pll_status();
  uint32_t number_of_endpoint_nodes = EndpointDesign<IO>::get_number_of_endpoint_nodes();
  for (uint32_t i = 0; i < number_of_endpoint_nodes; ++i) {
    status << "Endpoint node " << i << " status" << std::endl;
    status << this->get_endpoint_node(i).get_status();
  }
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
EndpointDesign<IO>::configure() const
{
  // Hard resets
  this->reset();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
EndpointDesign<IO>::get_info(opmonlib::InfoCollector& ci, int level) const
{ 
  opmonlib::InfoCollector endpoint_collector;
  this->get_endpoint_node(0).get_info(endpoint_collector, level);
  ci.add("endpoint", endpoint_collector);

  opmonlib::InfoCollector hardware_collector;
  this->get_io_node().get_info(hardware_collector, level);
  ci.add("io", hardware_collector);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
uint32_t
EndpointDesign<IO>::read_firmware_version() const
{
  return this->get_endpoint_node(0).read_version();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
EndpointDesign<IO>::validate_firmware_version() const
{
  auto firmware_version = read_firmware_version();
  
  int major_firmware_version = (firmware_version >> 16) & 0xff;
  int minor_firmware_version = (firmware_version >> 8) & 0xff;
  int patch_firmware_version = (firmware_version >> 0) & 0xff;

  if (major_firmware_version != g_required_major_endpoint_firmware_version)
    ers::error(IncompatibleMajorEndpointFirmwareVersion(ERS_HERE, major_firmware_version, g_required_major_endpoint_firmware_version));
  if (minor_firmware_version != g_required_minor_endpoint_firmware_version)
    ers::warning(IncompatibleMinorEndpointFirmwareVersion(ERS_HERE, minor_firmware_version, g_required_minor_endpoint_firmware_version));
  if (patch_firmware_version != g_required_patch_endpoint_firmware_version)
    ers::warning(IncompatiblePatchEndpointFirmwareVersion(ERS_HERE, patch_firmware_version, g_required_patch_endpoint_firmware_version));
}
//-----------------------------------------------------------------------------
}
