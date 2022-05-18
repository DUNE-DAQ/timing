#include <sstream>
#include <string>

namespace dunedaq::timing {

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
//template<class MST>
//uhal::Node*
//MasterMuxDesign<MST>::clone() const
//{
//  return new MasterMuxDesign<MST>(static_cast<const MasterMuxDesign<MST>&>(*this));
//}
////-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
MasterMuxDesign<MST>::MasterMuxDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MuxDesignInterface(node)
  , MasterDesignInterface(node)
  , MasterDesign<MST>(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
MasterMuxDesign<MST>::~MasterMuxDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
std::string
MasterMuxDesign<MST>::get_status(bool print_out) const
{
  std::stringstream status;
  status << get_io_node_plain()->get_pll_status();
  status << this->get_master_node().get_status();
  // TODO mux specific status
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
uint32_t
MasterMuxDesign<MST>::measure_endpoint_rtt(uint32_t address, bool control_sfp, int sfp_mux) const
{

  if (sfp_mux > -1) {
    if (control_sfp) 
    {
      // set fanout rtt mux channel, and do not wait for fanout rtt ept to be in a good state
      switch_downstream_mux_channel(sfp_mux, false);
    }
    // gets master rtt ept in a good state, and sends echo command
    uint32_t rtt = this->get_master_node().measure_endpoint_rtt(address, control_sfp);
    return rtt;
  } else {
    return this->get_master_node().measure_endpoint_rtt(address, control_sfp);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
void
MasterMuxDesign<MST>::apply_endpoint_delay(uint32_t address,
                                               uint32_t coarse_delay,
                                               uint32_t fine_delay,
                                               uint32_t phase_delay,
                                               bool measure_rtt,
                                               bool control_sfp,
                                               int sfp_mux) const
{
  if (sfp_mux > -1)
  {
    if (control_sfp && measure_rtt)
    {
      // set fanout rtt mux channel, and do not wait for fanout rtt ept to be in a good state
      switch_downstream_mux_channel(sfp_mux, false);  
    }
    // gets master rtt ept in a good state, and sends echo command
    this->get_master_node().apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay, measure_rtt, control_sfp);
  }
  else
  {
    this->get_master_node().apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay, measure_rtt, control_sfp);
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST>
void
MasterMuxDesign<MST>::switch_downstream_mux_channel(uint32_t sfp_id, bool wait_for_rtt_ept_lock) const
{
  TopDesignInterface::get_io_node<timing::FanoutIONode>()->switch_downstream_mux_channel(sfp_id);
  if (wait_for_rtt_ept_lock) {
    this->get_master_node().enable_upstream_endpoint();
  }
}
//-----------------------------------------------------------------------------


template<class MST>
std::vector<uint32_t>
MasterMuxDesign<MST>::scan_sfp_mux() const 
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

}