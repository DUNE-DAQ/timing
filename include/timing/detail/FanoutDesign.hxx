#include <sstream>
#include <string>

namespace dunedaq::timing {

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO, class MST>
uhal::Node*
FanoutDesign<IO, MST>::clone() const
{
  return new FanoutDesign<IO, MST>(static_cast<const FanoutDesign<IO, MST>&>(*this));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
FanoutDesign<IO, MST>::FanoutDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MuxDesignInterface(node)
  , MasterDesignInterface(node)
  , MasterMuxDesign<IO, MST>(node)
  , EndpointDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
FanoutDesign<IO, MST>::~FanoutDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
std::string
FanoutDesign<IO, MST>::get_status(bool print_out) const
{
  std::stringstream status;
  status << TopDesign<IO>::get_io_node().get_pll_status();
  status << MasterDesign<IO, MST>::get_master_node().get_status();
  // TODO fanout specific status
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
void
FanoutDesign<IO, MST>::configure() const
{
  // fanout mode hard-coded, to be passed in as parameter in future
  uint32_t fanout_mode = 0;

  // Hard reset
  this->reset_io(fanout_mode);

  if (!fanout_mode) {
    // Set timestamp to current time
    this->sync_timestamp();

    // Enable spill interface
    MasterDesign<IO, MST>::get_master_node().enable_spill_interface();
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
void
FanoutDesign<IO, MST>::reset_io(int32_t fanout_mode, const std::string& clock_config_file) const
{
  TopDesign<IO>::get_io_node().reset(fanout_mode, clock_config_file);
  // 0 - fanout mode, outgoing data comes from sfp
  // 1 - standalone mode, outgoing data comes from local master
  uhal::Node::getNode("switch.csr.ctrl.master_src").write(fanout_mode);
  uhal::Node::getClient().dispatch();
}
//-----------------------------------------------------------------------------

////-----------------------------------------------------------------------------
// template<class IO, class MST>
// void FanoutDesign<IO,MST>::reset(const std::string& clock_config_file) const {
//	this->reset(0, clock_config_file);
//}
////-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t
FanoutDesign<IO, MST>::measure_endpoint_rtt(uint32_t address, bool control_sfp, int sfp_mux) const
{
  auto fanout_mode = uhal::Node::getNode("switch.csr.ctrl.master_src").read();
  uhal::Node::getClient().dispatch();

  if (!fanout_mode.value()) {
    std::ostringstream message;
    message << " Fanout unit " << uhal::Node::getId()
         << " is in fanout mode. Measure endpoint RTT should be called from master device.";
    throw UnsupportedFunction(ERS_HERE, message.str());
  }
  return MasterMuxDesign<IO,MST>::measure_endpoint_rtt(address, control_sfp, sfp_mux);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
void
FanoutDesign<IO, MST>::apply_endpoint_delay(uint32_t address,
                                            uint32_t coarse_delay,
                                            uint32_t fine_delay,
                                            uint32_t phase_delay,
                                            bool measure_rtt,
                                            bool control_sfp,
                                            int sfp_mux) const
{
  auto fanout_mode = uhal::Node::getNode("switch.csr.ctrl.master_src").read();
  uhal::Node::getClient().dispatch();

  if (!fanout_mode.value()) {
    std::ostringstream message;
    message << "Fanout unit " << uhal::Node::getId()
         << " is in fanout mode. Apply endpoint delay should be called from master device.";
    throw UnsupportedFunction(ERS_HERE, message.str());
  }
  MasterMuxDesign<IO,MST>::apply_endpoint_delay(
    address, coarse_delay, fine_delay, phase_delay, measure_rtt, control_sfp, sfp_mux);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
void
FanoutDesign<IO, MST>::get_info(opmonlib::InfoCollector& ci, int level) const
{ 
  opmonlib::InfoCollector master_collector;
  this->get_master_node().get_info(master_collector, level);
  ci.add("master", master_collector);

  opmonlib::InfoCollector hardware_collector;
  this->get_io_node().get_info(hardware_collector, level);
  ci.add("io", hardware_collector);
}
//-----------------------------------------------------------------------------

}