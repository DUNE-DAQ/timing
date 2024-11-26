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
  , MuxDesignInterface(node)
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
GaiaDesign::configure(uint8_t source) const
{
  auto clock_source = static_cast<ClockSource>(source);
  // Hard reset
  TopDesign::configure(clock_source);

  if (clock_source == kFreeRun)
  {
    this->sync_timestamp(kSoftware); // keep previous behaviour for now, TODO: pass through correct parameter
  }
  else
  {
    // TODO: set irig epoch
    this->sync_timestamp(kUpstream);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
GaiaDesign::configure(uint8_t source, uint8_t epoch) const
{
  auto clock_source = static_cast<ClockSource>(source);
  // Hard reset
  TopDesign::configure(clock_source);

  if (clock_source == kFreeRun)
  {
    this->sync_timestamp(kSoftware); // keep previous behaviour for now, TODO: pass through correct parameter
  }
  else
  {
    getNode<IRIGTimestampNode>("irig_time_source")
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