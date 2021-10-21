#include <sstream>
#include <string>

namespace dunedaq::timing {

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node*
ChronosDesign<IO>::clone() const
{
  return new ChronosDesign<IO>(static_cast<const ChronosDesign<IO>&>(*this));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
ChronosDesign<IO>::ChronosDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , EndpointDesignInterface(node)
  , EndpointDesign<IO>(node)
  , HSIDesignInterface(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
ChronosDesign<IO>::~ChronosDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
std::string
ChronosDesign<IO>::get_status(bool print_out) const
{
  std::stringstream status;
  status << TopDesign<IO>::get_io_node().get_pll_status();
  status << this->get_hsi_node().get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
ChronosDesign<IO>::get_info(opmonlib::InfoCollector& ci, int level) const
{  
  opmonlib::InfoCollector hardware_collector;
  this->get_io_node().get_info(hardware_collector, level);
  ci.add("io", hardware_collector);

  opmonlib::InfoCollector hsi_collector;
  this->get_hsi_node().get_info(hsi_collector, level);
  ci.add("hsi", hsi_collector);
}
//-----------------------------------------------------------------------------
}