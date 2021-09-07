/**
 * @file TimingSystemManagerBase.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/TimingSystemManagerBase.hpp"

#include <string>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
TimingSystemManagerBase::TimingSystemManagerBase(std::string cf)
  : // eventually will be provided via parameters from a config file
  connectionsFile(cf)
  , masterHardwareNames({ "PROD_MASTER" })
  , endpointHardwareNames({ "EPT_2" })
  , mMaxMeasuredRTT(0)
{
  // mExpectedEndpoints.emplace("EPT_1", ActiveEndpointConfig("EPT_1", 1, -1, 0, 0x0));
  mExpectedEndpoints.emplace("EPT_2", ActiveEndpointConfig("EPT_2", 2, 0, 0, 0x0));
  // mExpectedEndpoints.emplace("EPT_3"] = ActiveEndpointConfig("EPT_3", 3, 0, 1));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
TimingSystemManagerBase::~TimingSystemManagerBase() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
TimingSystemManagerBase::getNumberOfMasters() const
{
  return masterHardware.size();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
TimingSystemManagerBase::getNumberOfEndpoints() const
{
  return endpointHardware.size();
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq