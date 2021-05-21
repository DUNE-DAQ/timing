/**
 * @file I2CExpanderNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/I2CExpanderNode.hpp"

// PDT headers
#include "ers/ers.hpp"
#include "timing/toolbox.hpp"

// #include <boost/tuple/tuple.hpp>

#include <vector>
// #include <fstream>
// #include <sstream>

namespace dunedaq {
namespace timing {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(I2CExpanderNode)

//-----------------------------------------------------------------------------
I2CExpanderSlave::I2CExpanderSlave(const I2CMasterNode* i2c_master, uint8_t address) // NOLINT(build/unsigned)
  : I2CSlave(i2c_master, address)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
I2CExpanderSlave::~I2CExpanderSlave() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CExpanderSlave::ensure_valid_bank_id(uint8_t bank_id) const // NOLINT(build/unsigned)
{
  if (bank_id == 0 || bank_id == 1)
    return;

  throw SFPExpanderBankIDError(ERS_HERE, std::to_string(bank_id));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CExpanderSlave::set_inversion(uint8_t bank_id, uint32_t inversion_mask) const // NOLINT(build/unsigned)
{

  this->ensure_valid_bank_id(bank_id);
  this->write_i2c(0x4 + bank_id, inversion_mask);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CExpanderSlave::set_io(uint8_t bank_id, uint32_t io_mask) const // NOLINT(build/unsigned)
{

  this->ensure_valid_bank_id(bank_id);
  this->write_i2c(0x6 + bank_id, io_mask);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
I2CExpanderSlave::set_outputs(uint8_t bank_id, uint32_t output_values) const // NOLINT(build/unsigned)
{

  this->ensure_valid_bank_id(bank_id);
  this->write_i2c(0x2 + bank_id, output_values);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t                                             // NOLINT(build/unsigned)
I2CExpanderSlave::read_inputs(uint8_t bank_id) const // NOLINT(build/unsigned)
{

  this->ensure_valid_bank_id(bank_id);
  return this->read_i2c(0x0 + bank_id);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<uint32_t> // NOLINT(build/unsigned)
I2CExpanderSlave::debug() const
{

  std::vector<uint32_t> lValues(8); // NOLINT(build/unsigned)

  for (size_t a(0); a < 8; ++a) {
    lValues[a] = this->read_i2c(a);
  }
  return lValues;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
I2CExpanderNode::I2CExpanderNode(const uhal::Node& node)
  : I2CMasterNode(node)
  , I2CExpanderSlave(this, this->get_slave_address("i2caddr"))
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
I2CExpanderNode::I2CExpanderNode(const I2CExpanderNode& node)
  : I2CMasterNode(node)
  , I2CExpanderSlave(this, this->get_slave_address("i2caddr"))
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
I2CExpanderNode::~I2CExpanderNode() {}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq