#include "timing/GaiaDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(GaiaDesign)

//-----------------------------------------------------------------------------
GaiaDesign::GaiaDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MasterDesignInterface(node)
  , MasterDesign(node)
  , EndpointDesignInterface(node)
  , CDRMuxDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
GaiaDesign::~GaiaDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
GaiaDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  status << MasterDesign::get_master_node_plain()->get_status();
  // TODO fanout specific status
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GaiaDesign::configure() const
{
  ClockSource clock_source = kInput0;
  // Hard reset
  this->reset_io(clock_source); // gaia normally takes clock from upstream GPS; add posibility override clock source via config in future

  if (clock_source == kFreeRun) {
    // Set timestamp to current time
    this->sync_timestamp();
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//void
//GaiaDesign::get_info(opmonlib::InfoCollector& /*ci*/, int /*level*/) const
//{ 
//  opmonlib::InfoCollector master_collector;
//  this->get_master_node_plain()->get_info(master_collector, level);
//  ci.add("master", master_collector);
//
//  opmonlib::InfoCollector hardware_collector;
//  this->get_io_node_plain()->get_info(hardware_collector, level);
//  ci.add("io", hardware_collector);
//
//  opmonlib::InfoCollector endpoint_collector;
//  get_endpoint_node_plain(0)->get_info(endpoint_collector, level);
//  ci.add("endpoint", endpoint_collector);
//}
//-----------------------------------------------------------------------------
