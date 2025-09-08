#include "timing/KerberosDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(KerberosDesign)

//-----------------------------------------------------------------------------
KerberosDesign::KerberosDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MasterDesignInterface(node)
  , MuxDesignInterface(node)
  , MasterDesign(node)
  , EndpointDesignInterface(node)
  , CDRMuxDesignInterface(node)
  , MasterMuxDesignInterface(node)
  , TimingSourceMuxDesignInterface(node)
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
KerberosDesign::configure(ClockSource clock_source, TimestampSource ts_source) const
{
  if (clock_source == kFreeRun)
  {
    TopDesign::configure(clock_source); // kerberos normally takes clock from upstream SFP
    this->sync_timestamp(ts_source);
  }
  else
  {
    switch_timing_source(clock_source);

    for (uint i=0; i <  get_number_of_endpoint_nodes(); ++i)
    {
      try
      {
        get_endpoint_node_plain(i)->reset(0x10+i);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        get_endpoint_node_plain(i)->get_status(true);

        if (!get_endpoint_node_plain(i)->endpoint_ready())
        {
          if (i==clock_source)
          {
            ers::error(EndpointNotReady(ERS_HERE, "MIB endpoint "+std::to_string(i)+" not ready!", get_endpoint_node_plain(i)->read_endpoint_state()));
          }
          else
          {
            ers::warning(EndpointNotReady(ERS_HERE, "MIB endpoint "+std::to_string(i)+" not ready!", get_endpoint_node_plain(i)->read_endpoint_state()));
          }
        }
      }
      catch (const std::exception& e)
      {
        if (i==clock_source)
        {
          ers::error(EndpointNotReady(ERS_HERE, "MIB endpoint "+std::to_string(i)+" has no clock!", get_endpoint_node_plain(i)->read_endpoint_state(),e));
        }
        else
        {
          ers::warning(EndpointNotReady(ERS_HERE, "MIB endpoint "+std::to_string(i)+" has no clock!", get_endpoint_node_plain(i)->read_endpoint_state(), e));
        }
      }
    }

    this->sync_timestamp(ts_source);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
KerberosDesign::switch_timing_source(ClockSource clock_source) const
{
  // Hard reset
  TopDesign::configure(clock_source); //TODO add option not to reprogram pll config

  switch_timing_source_mux(clock_source);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//void
//KerberosDesign::get_info(opmonlib::InfoCollector& ci, int level) const
//{ 
//  opmonlib::InfoCollector master_collector;
//  this->get_master_node_plain()->get_info(master_collector, level);
//  ci.add("master", master_collector);

//  opmonlib::InfoCollector hardware_collector;
//  this->get_io_node_plain()->get_info(hardware_collector, level);
//  ci.add("io", hardware_collector);

//  opmonlib::InfoCollector endpoint_collector;
//  get_endpoint_node_plain(0)->get_info(endpoint_collector, level);
//  ci.add("endpoint", endpoint_collector);
// }
//-----------------------------------------------------------------------------

}
