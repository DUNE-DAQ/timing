/**
 * @file SIMIONode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/SIMIONode.hpp"

#include "logging/Logging.hpp"

#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(SIMIONode)

//-----------------------------------------------------------------------------
SIMIONode::SIMIONode(const uhal::Node& node)
  : IONode(node, "", "", "", {}, {})
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SIMIONode::~SIMIONode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
SIMIONode::get_uid_address_parameter_name() const
{
  return "";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
SIMIONode::get_status(bool print_out) const
{
  std::stringstream status;

  auto subnodes = read_sub_nodes(getNode("csr.stat"));
  status << format_reg_table(subnodes, "SIM IO state");

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint64_t // NOLINT(build/unsigned)
SIMIONode::read_board_uid() const
{
  return 0;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
BoardRevision
SIMIONode::get_board_revision() const
{
  return kSIMRev1;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
SIMIONode::get_hardware_info(bool print_out) const
{
  std::stringstream info;
  const BoardType board_type = convert_value_to_board_type(read_board_type());
  const BoardRevision board_revision = get_board_revision();
  const CarrierType carrier_type = convert_value_to_carrier_type(read_carrier_type());
  const DesignType design_type = convert_value_to_design_type(read_design_type());

  std::vector<std::pair<std::string, std::string>> hardware_info;

  try {
    hardware_info.push_back(std::make_pair("Board type", get_board_type_map().at(board_type)));
  } catch (const std::out_of_range& e) {
    throw MissingBoardTypeMapEntry(ERS_HERE, format_reg_value(board_type), e);
  }

  try {
    hardware_info.push_back(std::make_pair("Board revision", get_board_revision_map().at(board_revision)));
  } catch (const std::out_of_range& e) {
    throw MissingBoardRevisionMapEntry(ERS_HERE, format_reg_value(board_revision), e);
  }

  try {
    hardware_info.push_back(std::make_pair("Carrier type", get_carrier_type_map().at(carrier_type)));
  } catch (const std::out_of_range& e) {
    throw MissingCarrierTypeMapEntry(ERS_HERE, format_reg_value(carrier_type), e);
  }

  try {
    hardware_info.push_back(std::make_pair("Design type", get_design_type_map().at(design_type)));
  } catch (const std::out_of_range& e) {
    throw MissingDesignTypeMapEntry(ERS_HERE, format_reg_value(design_type), e);
  }
  info << format_reg_table(hardware_info, "Hardware info", { "", "" });

  if (print_out)
    TLOG() << info.str();
  return info.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SIMIONode::reset(const std::string& /*clock_config_file*/) const
{

  write_soft_reset_register();
  TLOG_DEBUG(0) << "Reset done";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SIMIONode::reset(int32_t /*fanout_mode*/, // NOLINT(build/unsigned)
                     const std::string& clock_config_file) const
{
  reset(clock_config_file);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SIMIONode::configure_pll(const std::string& /*clock_config_file*/) const
{
  TLOG_DEBUG(0) << "Simulation does not support PLL config";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<double>
SIMIONode::read_clock_frequencies() const
{
  TLOG_DEBUG(0) << "Simulation does not support reading of freq";
  return {};
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
SIMIONode::get_clock_frequencies_table(bool /*print_out*/) const
{
  TLOG_DEBUG(0) << "Simulation does not support freq table";
  return "Simulation does not support freq table";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
SIMIONode::get_pll_status(bool /*print_out*/) const
{
  TLOG_DEBUG(0) << "Simulation does not support PLL status";
  return "Simulation does not support PLL status";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
SIMIONode::get_sfp_status(uint32_t /*sfp_id*/, bool /*print_out*/) const // NOLINT(build/unsigned)
{
  TLOG_DEBUG(0) << "Simulation does not support SFP I2C";
  return "Simulation does not support SFP I2C";
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SIMIONode::switch_sfp_soft_tx_control_bit(uint32_t /*sfp_id*/, bool /*turn_on*/) const // NOLINT(build/unsigned)
{
  TLOG_DEBUG(0) << "Simulation does not support SFP I2C";
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq