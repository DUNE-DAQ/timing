/**
 * @file HadesDesign.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/HadesDesign.hpp"

#include <sstream>
#include <string>

namespace dunedaq::timing {

UHAL_REGISTER_DERIVED_NODE(HadesDesign)

//-----------------------------------------------------------------------------
HadesDesign::HadesDesign(const uhal::Node& node)
  : TopDesignInterface(node)
  , EndpointDesignInterface(node)
  , ChronosDesign(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
HadesDesign::~HadesDesign()
{}
//-----------------------------------------------------------------------------
} // namespace dunedaq::timing
