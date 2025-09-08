#include "timing/MasterMuxDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(MasterMuxDesign)

//-----------------------------------------------------------------------------
MasterMuxDesign::MasterMuxDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MasterMuxDesignInterface(node)
  , MasterDesignInterface(node)
  , MuxDesignInterface(node)
  , SFPMuxDesignInterface(node)
  , MasterDesign(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MasterMuxDesign::~MasterMuxDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MasterMuxDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  status << get_master_node_plain()->get_status();
  // TODO mux specific status
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//std::vector<uint32_t>
//MasterMuxDesign::scan_sfp_mux() const 
//{
//  std::vector<uint32_t> locked_channels;

  // TODO will this be right for every fanout board, need to check the IO board
//  uint32_t number_of_mux_channels = 8;
//  for (uint32_t i = 0; i < number_of_mux_channels; ++i)
//  {
//    TLOG_DEBUG(0) << "Scanning slot " << i;
//
//    try
//    {
//      switch_mux(i);
//      this->get_master_node_plain()->enable_upstream_endpoint();
//    } catch (...) {
//      TLOG_DEBUG(0) << "Slot " << i << " not locked";
//    }
//    // TODO catch right except
//
//    TLOG_DEBUG(0) << "Slot " << i << " locked";
//    locked_channels.push_back(i);
//  }
//
//  if (locked_channels.size()) {
//    TLOG() << "Slots locked: " << vec_fmt(locked_channels);
//  } else {
//    TLOG() << "No slots locked";
//  }
//  return locked_channels;
//}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterMuxDesign::resync_active_cdr() const
{
  this->get_master_node_plain()->enable_upstream_endpoint();
}
//-----------------------------------------------------------------------------
}