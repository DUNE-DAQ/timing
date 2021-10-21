#include <sstream>
#include <string>

namespace dunedaq::timing {

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node*
CRTDesign<IO>::clone() const
{
  return new CRTDesign<IO>(static_cast<const CRTDesign<IO>&>(*this));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
CRTDesign<IO>::CRTDesign(const uhal::Node& node)
  : TopDesignInterface(node) 
  , EndpointDesignInterface(node)
  , EndpointDesign<IO>(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
CRTDesign<IO>::~CRTDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
std::string
CRTDesign<IO>::get_status(bool print_out) const
{
  std::stringstream status;
  status << this->get_io_node().get_pll_status();
  status << this->get_crt_node().get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
template<class T>
void
CRTDesign<IO>::get_info(T& data) const
{
  this->get_io_node().get_info(data.hardware_data);
//  this->get_hsi_node().get_info(data.hsi_data);
}
//-----------------------------------------------------------------------------
}