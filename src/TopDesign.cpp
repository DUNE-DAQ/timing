/**
 * @file TopDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/BoreasDesign.hpp"
#include "timing/CRTDesign.hpp"
#include "timing/EndpointDesign.hpp"
#include "timing/FanoutDesign.hpp"
#include "timing/OuroborosDesign.hpp"
#include "timing/OuroborosMuxDesign.hpp"
#include "timing/OverlordDesign.hpp"
#include "timing/ChronosDesign.hpp"

namespace dunedaq {
namespace timing {
// In leiu of UHAL_REGISTER_DERIVED_NODE

// CRT
uhal::RegistrationHelper<CRTDesign<FMCIONode>> CRTDesign_FMCIONode_RegistrationHelper(
  "CRTDesign<FMCIONode>");

// Endpoint
uhal::RegistrationHelper<EndpointDesign<FMCIONode>> EndpointDesign_FMCIONode_RegistrationHelper(
  "EndpointDesign<FMCIONode>");

// Master
uhal::RegistrationHelper<MasterDesign<FMCIONode, PDIMasterNode>>
  MasterDesign_FMCIONode_PDIMasterNode_RegistrationHelper("MasterDesign<FMCIONode,PDIMasterNode>");

uhal::RegistrationHelper<MasterMuxDesign<PC059IONode, PDIMasterNode>>
  MasterMuxDesign_PC059IONode_PDIMasterNode_RegistrationHelper("MasterMuxDesign<PC059IONode,PDIMasterNode>");

// Fanout
uhal::RegistrationHelper<FanoutDesign<PC059IONode, PDIMasterNode>>
  FanoutDesign_PC059IONode_PDIMasterNode_RegistrationHelper("FanoutDesign<PC059IONode,PDIMasterNode>");

uhal::RegistrationHelper<FanoutDesign<FIBIONode, PDIMasterNode>>
  FanoutDesign_FIBIONode_PDIMasterNode_RegistrationHelper("FanoutDesign<FIBIONode,PDIMasterNode>");

// Ouroboros (master with endpoint)
uhal::RegistrationHelper<OuroborosDesign<SIMIONode>> OuroborosDesign_SIMIONode_RegistrationHelper(
  "OuroborosDesign<SIMIONode>");
uhal::RegistrationHelper<OuroborosDesign<FMCIONode>> OuroborosDesign_FMCIONode_RegistrationHelper(
  "OuroborosDesign<FMCIONode>");
uhal::RegistrationHelper<OuroborosMuxDesign<PC059IONode>> OuroborosMuxDesign_PC059IONode_RegistrationHelper(
  "OuroborosMuxDesign<PC059IONode>");
uhal::RegistrationHelper<OuroborosMuxDesign<FIBIONode>> OuroborosMuxDesign_FIBIONode_RegistrationHelper(
  "OuroborosMuxDesign<FIBIONode>");

// Overlord (master with trigger message receiver)
uhal::RegistrationHelper<OverlordDesign<TLUIONode>> OverlordDesign_TLUIONode_RegistrationHelper(
  "OverlordDesign<TLUIONode>");
uhal::RegistrationHelper<OverlordDesign<FMCIONode>> OverlordDesign_FMCIONode_RegistrationHelper(
  "OverlordDesign<FMCIONode>");

// Boreas (master with HSI)
uhal::RegistrationHelper<BoreasDesign<FMCIONode>> BoreasDesign_FMCIONode_RegistrationHelper("BoreasDesign<FMCIONode>");
uhal::RegistrationHelper<BoreasDesign<TLUIONode>> BoreasDesign_TLUIONode_RegistrationHelper("BoreasDesign<TLUIONode>");
uhal::RegistrationHelper<BoreasDesign<SIMIONode>> BoreasDesign_SIMIONode_RegistrationHelper("BoreasDesign<SIMIONode>");

// Chronos (HSI endpoint)
uhal::RegistrationHelper<ChronosDesign<FMCIONode>> ChronosDesign_FMCIONode_RegistrationHelper("ChronosDesign<FMCIONode>");

} // namespace timing
} // namespace dunedaq