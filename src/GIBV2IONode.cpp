/**
 * @file GIBV2IONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/GIBV2IONode.hpp"

#include <string>
#include <math.h>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(GIBV2IONode)

//-----------------------------------------------------------------------------
GIBV2IONode::GIBV2IONode(const uhal::Node& node)
  : GIBIONode(node)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
GIBV2IONode::~GIBV2IONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
GIBV2IONode::clocks_ok() const
{
  std::stringstream status;

  auto states = read_sub_nodes(getNode("csr.stat"));
  bool pll_lol = states.find("clk_gen_lol")->second.value();
  //bool pll_interrupt = states.find("clk_gen_intr")->second.value();
  bool mmcm_ok = states.find("mmcm_ok")->second.value();

  TLOG_DEBUG(5) << "pll lol: " << pll_lol << ", mmcm ok: " << mmcm_ok;

  return !pll_lol && mmcm_ok;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GIBV2IONode::validate_sfp_id(uint32_t sfp_id) const { // NOLINT(build/unsigned)
  // on this board we have 7 SFPs
  if (sfp_id > 6) {
        throw InvalidSFPId(ERS_HERE, format_reg_value(sfp_id));
  }
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq
