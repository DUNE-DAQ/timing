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
FanoutDesign::configure(uint8_t source) const
{
  // Hard reset
  auto clock_source = static_cast<ClockSource>(source);
  this->reset_io(clock_source); // fanout design is nominally FIB with input from backplane
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
