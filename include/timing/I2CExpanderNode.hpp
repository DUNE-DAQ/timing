/**
 * @file I2CExpanderNode.hpp
 *
 * I2CExpanderSlave and I2CExpanderNode are classes providing an interface
 * to the I2C expander IC.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_I2CEXPANDERNODE_HPP_
#define TIMING_INCLUDE_TIMING_I2CEXPANDERNODE_HPP_

#include "timing/I2CMasterNode.hpp"
#include "timing/I2CSlave.hpp"

#include <map>
#include <string>
#include <vector>

namespace dunedaq {
ERS_DECLARE_ISSUE(timing, SFPExpanderBankIDError, " Invalid bank id: " << bank_id, ((std::string)bank_id))

namespace timing {

/**
 * @class      I2CExpanderSlave
 *
 * @brief      I2C slave class to control SFP expander chips.
 * @author     Alessandro Thea
 * @date       April 2018
 */
class I2CExpanderSlave : public I2CSlave
{
public:
  I2CExpanderSlave(const I2CMasterNode* i2c_master, uint8_t i2c_device_address); // NOLINT(build/unsigned)
  virtual ~I2CExpanderSlave();

  /**
   * @brief      Sets the inversion status for a set of channels.
   *
   * @param[in]  bank_id         A bank identifier
   * @param[in]  inversion_mask  A inversion mask
   */
  void set_inversion(uint8_t bank_id, uint32_t inversion_mask) const; // NOLINT(build/unsigned)

  /**
   * @brief      Set input/output mode for a set of channels
   *
   * @param[in]  bank_id  A bank identifier
   * @param[in]  io_mask  A io mask
   */
  void set_io(uint8_t bank_id, uint32_t io_mask) const; // NOLINT(build/unsigned)

  /**
   * @brief      Reads inputs values
   *
   * @param[in]  bank_id  A bank identifier
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t read_inputs(uint8_t bank_id) const; // NOLINT(build/unsigned)

  /**
   * @brief      Writes outputs values.
   *
   * @param[in]  bank_id  A bank identifier
   * @param[in]  output_values  A values
   */
  void set_outputs(uint8_t bank_id, uint32_t output_values) const; // NOLINT(build/unsigned)

  /**
   * @brief      Reads output values
   *
   * @param[in]  aBankId  A bank identifier
   *
   * @return     { description_of_the_return_value }
   */
  uint8_t read_outputs_config( uint8_t bank_id ) const; // NOLINT(build/unsigned)

  std::vector<uint32_t> debug() const; // NOLINT(build/unsigned)

private:
  void ensure_valid_bank_id(uint8_t bank_id) const; // NOLINT(build/unsigned)
};

/**
 * @class      I2CExpanderNode
 *
 * @brief      uhal::Node implementing single I2C Master Slave connection to
 *             control SFP expander chips.
 * @author     Alessandro Thea
 * @date       April 2018
 */
class I2CExpanderNode
  : public I2CMasterNode
  , public I2CExpanderSlave
{
  UHAL_DERIVEDNODE(I2CExpanderNode)
public:
  explicit I2CExpanderNode(const uhal::Node& node);
  I2CExpanderNode(const I2CExpanderNode& node);
  virtual ~I2CExpanderNode();
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_I2CEXPANDERNODE_HPP_