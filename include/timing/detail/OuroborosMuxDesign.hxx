#include <sstream>
#include <string>

namespace dunedaq::timing {

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node*
OuroborosMuxDesign<IO>::clone() const
{
  return new OuroborosMuxDesign<IO>(static_cast<const OuroborosMuxDesign<IO>&>(*this));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
OuroborosMuxDesign<IO>::OuroborosMuxDesign(const uhal::Node& node)
  : TopDesign<IO>(node)
  , MasterMuxDesign<IO, PDIMasterNode>(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
OuroborosMuxDesign<IO>::~OuroborosMuxDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
std::string
OuroborosMuxDesign<IO>::get_status(bool print_out) const
{
  std::stringstream status;
  status << this->get_io_node().get_pll_status();
  status << this->get_master_node().get_status();
  status << this->get_endpoint_node(0).get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
OuroborosMuxDesign<IO>::configure() const
{

  // Hard resets
  this->reset();

  // Set timestamp to current time
  this->sync_timestamp();

  // Enable spill interface
  this->get_master_node().enable_spill_interface();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
OuroborosMuxDesign<IO>::get_info(opmonlib::InfoCollector& ci, int level) const
{ 
  opmonlib::InfoCollector master_collector;
  this->get_master_node().get_info(master_collector, level);
  ci.add("master", master_collector);

  opmonlib::InfoCollector hardware_collector;
  this->get_io_node().get_info(hardware_collector, level);
  ci.add("io", hardware_collector);

  opmonlib::InfoCollector endpoint_collector;
  this->get_endpoint_node(0).get_info(endpoint_collector, level);
  ci.add("master", endpoint_collector);
}
//-----------------------------------------------------------------------------
}