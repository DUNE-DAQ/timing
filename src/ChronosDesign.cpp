/**
 * @file ChronosDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/ChronosDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(ChronosDesign)

//-----------------------------------------------------------------------------
ChronosDesign::ChronosDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , EndpointDesignInterface(node)
  , TopDesign(node)
  , HSIDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
ChronosDesign::~ChronosDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
ChronosDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  status << get_endpoint_node_plain(0)->get_status();
  status << get_hsi_node().get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
ChronosDesign::configure() const
{
  // Hard resets
  this->reset_io(kInput1); // chronos FMC SFP is normally on input 1; add posibility override clock source via config in future
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// void
// ChronosDesign::get_info(opmonlib::InfoCollector& ci, int level) const
// {  
//   opmonlib::InfoCollector hardware_collector;
//   get_io_node_plain()->get_info(hardware_collector, level);
//   ci.add("io", hardware_collector);

//   opmonlib::InfoCollector endpoint_collector;
//   get_endpoint_node_plain(0)->get_info(endpoint_collector, level);
//   ci.add("endpoint", endpoint_collector);

//   opmonlib::InfoCollector hsi_collector;
//   get_hsi_node().get_info(hsi_collector, level);
//   ci.add("hsi", hsi_collector);
// }
//-----------------------------------------------------------------------------
} // namespace dunedaq::timing
