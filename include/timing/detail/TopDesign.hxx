#ifndef TIMING_INCLUDE_TIMING_TOPDESIGN_HXX_
#define TIMING_INCLUDE_TIMING_TOPDESIGN_HXX_

#include <sstream>
#include <string>

namespace dunedaq::timing {

//-----------------------------------------------------------------------------
template<class IO>
TopDesign<IO>::TopDesign(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
TopDesign<IO>::~TopDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
TopDesign<IO>::soft_reset() const
{
  get_io_node().soft_reset();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
TopDesign<IO>::reset(const std::string& clock_config_file) const
{
  get_io_node().reset(clock_config_file);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
const IO&
TopDesign<IO>::get_io_node() const
{
  return uhal::Node::getNode<IO>("io");
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
std::string
TopDesign<IO>::get_hardware_info(bool print_out) const
{
  auto lInfo = get_io_node().get_hardware_info();
  if (print_out)
    std::cout << lInfo;
  return lInfo;
}
//-----------------------------------------------------------------------------
}

#endif // TIMING_INCLUDE_TIMING_TOPDESIGN_HXX_