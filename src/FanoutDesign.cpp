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
FanoutDesign::configure() const
{
  // Hard reset
  this->reset_io(kInput1); // fanout design is nominally FIB with input from backplane; add posibility override clock source via config in future
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
