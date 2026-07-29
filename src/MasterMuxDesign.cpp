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
void
MasterMuxDesign::resync_active_cdr() const
{
  this->get_master_node_plain()->enable_upstream_endpoint();
}
//-----------------------------------------------------------------------------
}