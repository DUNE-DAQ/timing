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

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SFPEXPANDERNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SFPEXPANDERNODE_HPP_

#include "pdt/I2CSlave.hpp"
#include "pdt/I2CMasterNode.hpp"

#include <map>

namespace dunedaq {
ERS_DECLARE_ISSUE(pdt,
                  SFPExpanderBankIDError,
                  " Invalid bank id: " << bank_id,
                  ((std::string)bank_id)
)

namespace pdt
{

/**
 * @class      I2CExpanderSlave
 *
 * @brief      I2C slave class to control SFP expander chips.
 * @author     Alessandro Thea
 * @date       April 2018
 */
class I2CExpanderSlave : public I2CSlave {
public:
    I2CExpanderSlave(const I2CMasterNode* i2c_master, uint8_t i2c_device_address);
    virtual ~I2CExpanderSlave();
  
    /**
     * @brief      Sets the inversion status for a set of channels.
     *
     * @param[in]  bank_id         A bank identifier
     * @param[in]  inversion_mask  A inversion mask
     */
    void set_inversion( uint8_t bank_id, uint32_t inversion_mask ) const;

    /**
     * @brief      Set input/output mode for a set of channels
     *
     * @param[in]  bank_id  A bank identifier
     * @param[in]  io_mask  A io mask
     */
    void set_io( uint8_t bank_id, uint32_t io_mask ) const;

    /**
     * @brief      Reads inputs values
     *
     * @param[in]  bank_id  A bank identifier
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t read_inputs( uint8_t bank_id ) const;

    /**
     * @brief      Writes outputs values.
     *
     * @param[in]  bank_id  A bank identifier
     * @param[in]  output_values  A values
     */
    void set_outputs( uint8_t bank_id,  uint32_t output_values ) const;

    std::vector<uint32_t> debug() const;

private:

    void ensure_valid_bank_id( uint8_t bank_id ) const;

};

/**
 * @class      I2CExpanderNode
 *
 * @brief      uhal::Node implementing single I2C Master Slave connection to
 *             control SFP expander chips.
 * @author     Alessandro Thea
 * @date       April 2018
 */
class I2CExpanderNode : public I2CMasterNode, public I2CExpanderSlave {
    UHAL_DERIVEDNODE(I2CExpanderNode)
public:
    I2CExpanderNode(const uhal::Node& node);
    I2CExpanderNode(const I2CExpanderNode& node);
    virtual ~I2CExpanderNode();

};


} // namespace pdt
} // namespace dunedaq

#endif /* TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SFPEXPANDERNODE_HPP_ */
