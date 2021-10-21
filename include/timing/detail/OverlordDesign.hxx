#include <sstream>
#include <string>

namespace dunedaq::timing {

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node*
OverlordDesign<IO>::clone() const
{
  return new OverlordDesign<IO>(static_cast<const OverlordDesign<IO>&>(*this));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
OverlordDesign<IO>::OverlordDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , MasterDesignInterface(node)
  , OverlordDesignInterface(node)
  , MasterDesign<IO, PDIMasterNode>(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
OverlordDesign<IO>::~OverlordDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
std::string
OverlordDesign<IO>::get_status(bool print_out) const
{
  std::stringstream status;
  status << timing::TopDesign<IO>::get_io_node().get_pll_status();
  status << timing::MasterDesign<IO, PDIMasterNode>::get_master_node().get_status();
  status << this->get_external_triggers_endpoint_node().get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
OverlordDesign<IO>::configure() const
{

  // Hard resets
  this->reset_io();

  // Set timestamp to current time
  this->sync_timestamp();

  // Enable spill interface
  timing::MasterDesign<IO, PDIMasterNode>::get_master_node().enable_spill_interface();

  // Trigger interface configuration
  this->reset_external_triggers_endpoint();
  this->enable_external_triggers();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
OverlordDesign<IO>::get_info(opmonlib::InfoCollector& ci, int level) const
{ 
  opmonlib::InfoCollector master_collector;
  this->get_master_node().get_info(master_collector, level);
  ci.add("master", master_collector);

  opmonlib::InfoCollector hardware_collector;
  this->get_io_node().get_info(hardware_collector, level);
  ci.add("io", hardware_collector);

  // TODO full trix info
  //auto trig_interface_enabled = uhal::Node::getNode("trig_rx.csr.ctrl.ext_trig_en").read();
  //uhal::Node::getClient().dispatch();
  //data.trig_interface_enabled = trig_interface_enabled.value();
}
//-----------------------------------------------------------------------------

}