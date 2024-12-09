/**
 * @file CDCLVD110Node.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/CDCLVD110Node.hpp"

#include "logging/Logging.hpp"

#include <string>
#include <chrono>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(CDCLVD110Node)

//-----------------------------------------------------------------------------
CDCLVD110Node::CDCLVD110Node(const uhal::Node& node)
  : ClockGeneratorInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
CDCLVD110Node::~CDCLVD110Node() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
CDCLVD110Node::get_status(bool print_out) const
{
  std::stringstream status;
  //auto subnodes = read_sub_nodes(getNode("csr.stat"));
  //status << format_reg_table(subnodes, "CDCLVD110Node state");
  status << "CDCLVD110Node state"; //TODO: implement
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void // NOLINT(build/unsigned)
CDCLVD110Node::get_info(timinghardwareinfo::TimingPLLMonitorData& mon_data) const
{
  mon_data.lol = false; // no monitoring of this in CDCLVD110
  mon_data.los = false;
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq