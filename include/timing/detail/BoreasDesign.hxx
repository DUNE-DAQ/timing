#include <sstream>
#include <string>

namespace dunedaq::timing {

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node*
BoreasDesign<IO>::clone() const
{
  return new BoreasDesign<IO>(static_cast<const BoreasDesign<IO>&>(*this));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
BoreasDesign<IO>::BoreasDesign(const uhal::Node& node)
  : TopDesign<IO>(node)
  , MasterDesign<IO, PDIMasterNode>(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
BoreasDesign<IO>::~BoreasDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
std::string
BoreasDesign<IO>::get_status(bool print_out) const
{
  std::stringstream status;
  status << this->get_io_node().get_pll_status();
  status << this->get_master_node().get_status();
  status << this->get_hsi_node().get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
BoreasDesign<IO>::configure() const
{

  // Hard resets
  this->reset();

  // Set timestamp to current time
  this->sync_timestamp();

  // configure hsi
  // this->get_his_node().
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
template<class T>
void
BoreasDesign<IO>::get_info(T& data) const
{
  this->get_master_node().get_info(data.master_data);
  this->get_io_node().get_info(data.hardware_data);
  this->get_hsi_node().get_info(data.hsi_data);
}
//-----------------------------------------------------------------------------
}