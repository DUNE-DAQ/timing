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
  : TopDesign<IO>(node), EndpointDesign<IO>(node)
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
  status << this->get_io_node().get_pll_status();
  status << this->get_hsi_node().get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
template<class T>
void
ChronosDesign<IO>::get_info(T& data) const
{
  this->get_io_node().get_info(data.hardware_data);
  this->get_hsi_node().get_info(data.hsi_data);
}
//-----------------------------------------------------------------------------
}