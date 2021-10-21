#include <sstream>
#include <string>

#include "timing/MasterMuxDesignInterface.hpp"


namespace dunedaq::timing {

//-----------------------------------------------------------------------------
MasterMuxDesignInterface::MasterMuxDesignInterface(const uhal::Node& node)
  : MasterDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
MasterMuxDesignInterface::~MasterMuxDesignInterface()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
MasterMuxDesignInterface::get_status(bool print_out) const
{
  std::stringstream status;
  status << TopDesignInterface::get_io_node<timing::IONode>()->get_pll_status();
  status << MasterDesignInterface::get_master_node<timing::MasterNode>()->get_status();
  // TODO mux specific status
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterMuxDesignInterface::switch_sfp_mux_channel(uint32_t sfp_id, bool wait_for_rtt_ept_lock) const
{
  TopDesignInterface::get_io_node<timing::FanoutIONode>()->switch_sfp_mux_channel(sfp_id);
  if (wait_for_rtt_ept_lock) {
    MasterDesignInterface::get_master_node<timing::MasterNode>()->enable_upstream_endpoint();
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t
MasterMuxDesignInterface::read_active_sfp_mux_channel() const
{
  return TopDesignInterface::get_io_node<timing::FanoutIONode>()->read_active_sfp_mux_channel();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t
MasterMuxDesignInterface::measure_endpoint_rtt(uint32_t address, bool control_sfp, uint32_t sfp_mux) const
{

  if (control_sfp) {
    MasterDesignInterface::get_master_node<timing::MasterNode>()->switch_endpoint_sfp(0x0, false);
    MasterDesignInterface::get_master_node<timing::MasterNode>()->switch_endpoint_sfp(address, true);
  }

  // set fanout rtt mux channel, and do not wait for fanout rtt ept to be in a good state
  this->switch_sfp_mux_channel(sfp_mux, false);

  // sleep for a short time, otherwise the rtt endpoint will not get state to 0x8 in time
  millisleep(200);

  // gets master rtt ept in a good state, and sends echo command (due to second argument endpoint sfp is not controlled
  // in this call, already done above)
  uint32_t rtt = MasterDesignInterface::get_master_node<timing::MasterNode>()->measure_endpoint_rtt(address, false);

  if (control_sfp)
    MasterDesignInterface::get_master_node<timing::MasterNode>()->switch_endpoint_sfp(address, false);
  return rtt;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
MasterMuxDesignInterface::apply_endpoint_delay(uint32_t address,
                                               uint32_t coarse_delay,
                                               uint32_t fine_delay,
                                               uint32_t phase_delay,
                                               bool measure_rtt,
                                               bool control_sfp,
                                               uint32_t sfp_mux) const
{

  if (measure_rtt) {
    if (control_sfp) {
      MasterDesignInterface::get_master_node<timing::MasterNode>()->switch_endpoint_sfp(0x0, false);
      MasterDesignInterface::get_master_node<timing::MasterNode>()->switch_endpoint_sfp(address, true);
    }

    // set fanout rtt mux channel, and wait for fanout rtt ept to be in a good state, don't bother waiting for a good
    // rtt endpoint, the next method call takes care of that
    this->switch_sfp_mux_channel(sfp_mux, false);
  }

  MasterDesignInterface::get_master_node<timing::MasterNode>()->apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay, measure_rtt, false);

  if (measure_rtt && control_sfp)
    MasterDesignInterface::get_master_node<timing::MasterNode>()->switch_endpoint_sfp(address, false);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint32_t>
MasterMuxDesignInterface::scan_sfp_mux() const
{
  std::vector<uint32_t> locked_channels;

  // TODO will this be right for every fanout board, need to check the IO board
  uint32_t number_of_mux_channels = 8;
  for (uint32_t i = 0; i < number_of_mux_channels; ++i) {
    TLOG_DEBUG(0) << "Scanning slot " << i;

    try {
      this->switch_sfp_mux_channel(i, true);
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