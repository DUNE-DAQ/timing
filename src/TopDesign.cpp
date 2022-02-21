/**
 * @file TopDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PDIMasterNode.hpp"

//#include "timing/BoreasDesign.hpp"
//#include "timing/ChronosDesign.hpp"
//#include "timing/CRTDesign.hpp"
//#include "timing/EndpointDesign.hpp"
#include "timing/MasterDesign.hpp"
#include "timing/MasterMuxDesign.hpp"
#include "timing/FanoutDesign.hpp"
//#include "timing/OuroborosDesign.hpp"
//#include "timing/OuroborosMuxDesign.hpp"
//#include "timing/OverlordDesign.hpp"

namespace dunedaq {
namespace timing {
// In leiu of UHAL_REGISTER_DERIVED_NODE

// CRT
//uhal::RegistrationHelper<CRTDesign> CRTDesign_RegistrationHelper("CRTDesign");

// Endpoint
//uhal::RegistrationHelper<EndpointDesign> EndpointDesign_RegistrationHelper("EndpointDesign");

// Master
uhal::RegistrationHelper<MasterDesign<PDIMasterNode>> MasterDesign_PDIMasterNode_RegistrationHelper("MasterDesign<PDIMasterNode>");
uhal::RegistrationHelper<MasterMuxDesign<PDIMasterNode>> MasterMuxDesign_PDIMasterNode_RegistrationHelper("MasterMuxDesign<PDIMasterNode>");
// Fanout
uhal::RegistrationHelper<FanoutDesign<PDIMasterNode>> FanoutDesign_PDIMasterNode_RegistrationHelper("FanoutDesign<PDIMasterNode>");

// Ouroboros (master with endpoint)
//uhal::RegistrationHelper<OuroborosDesign> OuroborosDesign_RegistrationHelper("OuroborosDesign");

//uhal::RegistrationHelper<OuroborosMuxDesign> OuroborosMuxDesign_RegistrationHelper("OuroborosMuxDesign");

// Overlord (master with trigger message receiver)
//uhal::RegistrationHelper<OverlordDesign> OverlordDesign_RegistrationHelper("OverlordDesign");

// Boreas (master with HSI)
//uhal::RegistrationHelper<BoreasDesign> BoreasDesign_RegistrationHelper("BoreasDesign");

//uhal::RegistrationHelper<ChronosDesign> ChronosDesign_RegistrationHelper("ChronosDesign");

} // namespace timing
} // namespace dunedaq