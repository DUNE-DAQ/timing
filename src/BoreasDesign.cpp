/**
 * @file BoreasDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/BoreasDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(BoreasDesign)

//-----------------------------------------------------------------------------
BoreasDesign::BoreasDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MasterDesignInterface(node)
  , EndpointDesignInterface(node)
  , MasterDesign(node)
  , HSIDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
BoreasDesign::~BoreasDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
BoreasDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  status << get_master_node_plain()->get_status();
  status << get_endpoint_node_plain(0)->get_status();
  status << get_hsi_node().get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
BoreasDesign::configure(ClockSource clock_source, TimestampSource ts_source) const
{
  MasterDesign::configure(clock_source, ts_source);
  // configure endpoint
  // configure hsi
  // get_his_node().
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
BoreasDesign::get_info(timingfirmwareinfo::TimingDeviceInfo& mon_data) const
{
  MasterDesign::get_info(mon_data);
  HSIDesignInterface::get_info(mon_data);
}
//-----------------------------------------------------------------------------
} // namespace dunedaq::timing
