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
  : TopDesign<IO>(node)
  , MasterDesign<IO, PDIMasterNode>(node)
  , EndpointDesign<IO>(node)
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
  std::stringstream lStatus;
  lStatus << this->get_io_node().get_pll_status();
  lStatus << this->get_master_node().get_status();
  lStatus << this->get_external_triggers_endpoint_node().get_status();
  if (print_out)
    std::cout << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
OverlordDesign<IO>::configure() const
{

  // Hard resets
  this->reset();

  // Set timestamp to current time
  this->get_master_node().sync_timestamp();

  // Enable spill interface
  this->get_master_node().enable_spill_interface();

  // Trigger interface configuration
  this->reset_external_triggers_endpoint();
  this->enable_external_triggers();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
const TriggerReceiverNode&
OverlordDesign<IO>::get_external_triggers_endpoint_node() const
{
  return uhal::Node::getNode<TriggerReceiverNode>("trig_rx");
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
OverlordDesign<IO>::reset_external_triggers_endpoint() const
{
  this->get_external_triggers_endpoint_node().reset();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
OverlordDesign<IO>::enable_external_triggers() const
{
  this->get_external_triggers_endpoint_node().enable_triggers();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
OverlordDesign<IO>::disable_external_triggers() const
{
  this->get_external_triggers_endpoint_node().disable_triggers();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
template<class T>
void
OverlordDesign<IO>::get_info(T& data) const
{
  this->get_master_node().get_info(data.master_data);
  this->get_io_node().get_info(data.hardware_data);

  // TODO full trix info
  auto trig_interface_enabled = uhal::Node::getNode("trig_rx.csr.ctrl.ext_trig_en").read();
  data.trig_interface_enabled = trig_interface_enabled.value();
}
//-----------------------------------------------------------------------------

}