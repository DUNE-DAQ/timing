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

  if (clock_source == kFreeRun)
  {
    this->sync_timestamp(kSoftware); // keep previous behaviour for now, TODO: pass through correct parameter
  }
  else
  {
    this->sync_timestamp(kUpstream);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GaiaDesign::get_info(timingfirmwareinfo::TimingDeviceInfo& mon_data) const
{
  MasterDesign::get_info(mon_data);
  //EndpointDesignInterface::get_info(0, mon_data.endpoint_info);
}
//-----------------------------------------------------------------------------

}