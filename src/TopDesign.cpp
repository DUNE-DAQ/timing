/**
 * @file TopDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PDIMasterNode.hpp"

#include "timing/MasterDesign.hpp"
#include "timing/MasterMuxDesign.hpp"
#include "timing/FanoutDesign.hpp"

namespace dunedaq {
namespace timing {
// In leiu of UHAL_REGISTER_DERIVED_NODE

// Master
uhal::RegistrationHelper<MasterDesign<PDIMasterNode>> MasterDesign_PDIMasterNode_RegistrationHelper("MasterDesign<PDIMasterNode>");
uhal::RegistrationHelper<MasterMuxDesign<PDIMasterNode>> MasterMuxDesign_PDIMasterNode_RegistrationHelper("MasterMuxDesign<PDIMasterNode>");
// Fanout
uhal::RegistrationHelper<FanoutDesign<PDIMasterNode>> FanoutDesign_PDIMasterNode_RegistrationHelper("FanoutDesign<PDIMasterNode>");

} // namespace timing
} // namespace dunedaq