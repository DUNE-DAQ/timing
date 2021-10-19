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
void
ChronosDesign<IO>::configure_hsi(uint32_t src,      // NOLINT(build/unsigned)
                                uint32_t re_mask,  // NOLINT(build/unsigned)
                                uint32_t fe_mask,  // NOLINT(build/unsigned)
                                uint32_t inv_mask, // NOLINT(build/unsigned)
                                double rate,
                                bool dispatch) const
{
  uint32_t firmware_frequency = this->get_io_node().read_firmware_frequency();
  this->get_hsi_node().configure_hsi(src, re_mask, fe_mask, inv_mask, rate, firmware_frequency, dispatch);
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