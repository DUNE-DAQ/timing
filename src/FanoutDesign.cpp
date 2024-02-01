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
FanoutDesign::get_info(opmonlib::InfoCollector& ci, int level) const
{
  opmonlib::InfoCollector hardware_collector;
  this->get_io_node_plain()->get_info(hardware_collector, level);
  ci.add("io", hardware_collector);

  opmonlib::InfoCollector endpoint_collector;
  get_endpoint_node_plain(0)->get_info(endpoint_collector, level);
  ci.add("endpoint", endpoint_collector);
}
//-----------------------------------------------------------------------------

}