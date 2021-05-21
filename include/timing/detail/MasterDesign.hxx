#include <sstream>
#include <string>

namespace dunedaq::timing {

//-----------------------------------------------------------------------------
template<class IO, class MST>
MasterDesign<IO, MST>::MasterDesign(const uhal::Node& node)
  : TopDesign<IO>(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
MasterDesign<IO, MST>::~MasterDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
const MST&
MasterDesign<IO, MST>::get_master_node() const
{
  return uhal::Node::getNode<MST>("master");
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
uint64_t
MasterDesign<IO, MST>::read_master_timestamp() const
{
  return get_master_node().read_timestamp();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t
MasterDesign<IO, MST>::measure_endpoint_rtt(uint32_t address, bool control_sfp) const
{
  return get_master_node().measure_endpoint_rtt(address, control_sfp);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO, class MST>
void
MasterDesign<IO, MST>::apply_endpoint_delay(uint32_t address,
                                            uint32_t coarse_delay,
                                            uint32_t fine_delay,
                                            uint32_t phase_delay,
                                            bool measure_rtt,
                                            bool control_sfp) const
{
  get_master_node().apply_endpoint_delay(address, coarse_delay, fine_delay, phase_delay, measure_rtt, control_sfp);
}
//-----------------------------------------------------------------------------
}