#include "timing/MasterMuxDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(MasterMuxDesign)

//-----------------------------------------------------------------------------
MasterMuxDesign::MasterMuxDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MuxDesignInterface(node)
  , MasterDesignInterface(node)
  , MasterDesign(node)
{
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MasterMuxDesign::~MasterMuxDesign() {}
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
uint32_t
MasterMuxDesign::measure_endpoint_rtt(uint32_t address, bool control_sfp, int sfp_mux) const
{

  if (sfp_mux > -1) {
    if (control_sfp) {
      // set fanout rtt mux channel, and do not wait for fanout rtt ept to be in a good state
      switch_downstream_mux_channel(sfp_mux, false);
    }
    // gets master rtt ept in a good state, and sends echo command
    uint32_t rtt = get_master_node_plain()->measure_endpoint_rtt(address, control_sfp);
    return rtt;
  } else {
    return get_master_node_plain()->measure_endpoint_rtt(address, control_sfp);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterMuxDesign::apply_endpoint_delay(uint32_t address,
                                      uint32_t coarse_delay,
                                      uint32_t fine_delay,
                                      uint32_t phase_delay,
                                      bool measure_rtt,
                                      bool control_sfp,
                                      int sfp_mux) const
{
  if (sfp_mux > -1) {
    if (control_sfp && measure_rtt) {
      // set fanout rtt mux channel, and do not wait for fanout rtt ept to be in a good state
      switch_downstream_mux_channel(sfp_mux, false);
    }
    // gets master rtt ept in a good state, and sends echo command
    get_master_node_plain()->apply_endpoint_delay(
      address, coarse_delay, fine_delay, phase_delay, measure_rtt, control_sfp);
  } else {
    get_master_node_plain()->apply_endpoint_delay(
      address, coarse_delay, fine_delay, phase_delay, measure_rtt, control_sfp);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterMuxDesign::switch_downstream_mux_channel(uint32_t sfp_id, bool wait_for_rtt_ept_lock) const
{
  TopDesignInterface::get_io_node<timing::FanoutIONode>()->switch_downstream_mux_channel(sfp_id);
  if (wait_for_rtt_ept_lock) {
    this->get_master_node_plain()->enable_upstream_endpoint();
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint32_t>
MasterMuxDesign::scan_sfp_mux() const
{
  std::vector<uint32_t> locked_channels;

  // TODO will this be right for every fanout board, need to check the IO board
  uint32_t number_of_mux_channels = 8;
  for (uint32_t i = 0; i < number_of_mux_channels; ++i) {
    TLOG_DEBUG(0) << "Scanning slot " << i;

    try {
      switch_downstream_mux_channel(i, true);
    } catch (...) {
      TLOG_DEBUG(0) << "Slot " << i << " not locked";
    }
    // TODO catch right except

    TLOG_DEBUG(0) << "Slot " << i << " locked";
    locked_channels.push_back(i);
  }

  if (locked_channels.size()) {
    TLOG() << "Slots locked: " << vec_fmt(locked_channels);
  } else {
    TLOG() << "No slots locked";
  }
  return locked_channels;
}
//-----------------------------------------------------------------------------
}