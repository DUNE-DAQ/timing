/**
 * @file CRTDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/CRTDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(CRTDesign)

//-----------------------------------------------------------------------------
CRTDesign::CRTDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , EndpointDesignInterface(node)
  , TopDesign(node)
  , CRTDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
CRTDesign::~CRTDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
CRTDesign::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  status << get_crt_node().get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
CRTDesign::configure() const
{
  // Hard resets
  reset_io();
}
//-----------------------------------------------------------------------------

} // namespace dunedaq::timing