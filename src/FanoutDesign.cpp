#include "timing/FanoutDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(FanoutDesign)

//-----------------------------------------------------------------------------
FanoutDesign::FanoutDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , TopDesign(node)
  , EndpointDesignInterface(node)
  , MuxDesignInterface(node)
  , CDRMuxDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
FanoutDesign::~FanoutDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
FanoutDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  // TODO fanout specific status
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FanoutDesign::validate_firmware_version() const
{
  TLOG() << "Firmware version not available in fanout";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t
FanoutDesign::read_firmware_version() const
{
  TLOG() << "Firmware version not available in fanout";
  return 0;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FanoutDesign::configure(ClockSource clock_source) const
{
  // Hard reset
  TopDesign::configure(clock_source); // fanout design is nominally FIB with input from backplane

  get_endpoint_node_plain(0)->reset(0x20); //TODO set correct address
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  get_endpoint_node_plain(0)->get_status(true);
  if (!get_endpoint_node_plain(0)->endpoint_ready())
  {
    throw EndpointNotReady(ERS_HERE, "Fanout endpoint not ready!", get_endpoint_node_plain(0)->read_endpoint_state());
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
FanoutDesign::get_info(timingfirmwareinfo::TimingDeviceInfo& mon_data) const
{
  TopDesign::get_info(mon_data);
  EndpointDesignInterface::get_info(0, mon_data.endpoint_info);
}
//-----------------------------------------------------------------------------

}
