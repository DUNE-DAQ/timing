#include "timing/KerberosDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(KerberosDesign)

//-----------------------------------------------------------------------------
KerberosDesign::KerberosDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MuxDesignInterface(node)
  , MasterDesignInterface(node)
  , MasterMuxDesign(node)
  , EndpointDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
KerberosDesign::~KerberosDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
KerberosDesign::get_status(bool print_out) const
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
KerberosDesign::configure() const
{
  ClockSource clock_source = kInput0;
  // Hard reset
  this->reset_io(clock_source); // kerberos normally takes clock from upstream SFP, firmware selectable; add posibility override clock source via config in future

  if (clock_source == kFreeRun) {
    // Set timestamp to current time
    this->sync_timestamp();
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t
KerberosDesign::measure_endpoint_rtt(uint32_t address, bool control_sfp, int sfp_mux) const
{
  return MasterMuxDesign::measure_endpoint_rtt(address, control_sfp, sfp_mux);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
KerberosDesign::apply_endpoint_delay(uint32_t address,
                                            uint32_t coarse_delay,
                                            uint32_t fine_delay,
                                            uint32_t phase_delay,
                                            bool measure_rtt,
                                            bool control_sfp,
                                            int sfp_mux) const
{
  MasterMuxDesign::apply_endpoint_delay(
    address, coarse_delay, fine_delay, phase_delay, measure_rtt, control_sfp, sfp_mux);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
KerberosDesign::get_info(opmonlib::InfoCollector& /*ci*/, int /*level*/) const
{ 
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
}
//-----------------------------------------------------------------------------

}
