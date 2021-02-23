/**
 * @file DACNode.hpp
 *
 * DACNode and DACSlaves are classes providing an interface
 * to the physical DAC IC.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_DACNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_DACNODE_HPP_

#include "pdt/I2CSlave.hpp"
#include "pdt/I2CMasterNode.hpp"

#include "ers/ers.h"

namespace dunedaq {

ERS_DECLARE_ISSUE(pdt,                                      ///< Namespace
                  DACChannelOutOfRange,                     ///< Issue class name
                  " DAC channel out of range: " << channel, ///< Message
                  ((std::string)channel)                    ///< Message parameters
)
ERS_DECLARE_ISSUE(pdt,                                  ///< Namespace
                  DACValueOutOfRange,                   ///< Issue class name
                  " DAC value out of range: " << value, ///< Message
                  ((std::string)value)                  ///< Message parameters
)

namespace pdt {

/**
 * @brief      Class for dac node.
 */
class DACSlave : public I2CSlave
{
public:
    DACSlave(const I2CMasterNode* i2c_master, uint8_t i2c_device_address);
    virtual ~DACSlave() = default;
    
    /**
     * @brief     Use DAC internal reference
     */
    void set_interal_ref( bool internal_ref ) const;

    /**
     * @brief     Configure DAC channel
     */
    void set_dac(uint8_t channel, uint32_t code) const;
};

/**
 * @class      DACNode
 *
 * @brief      uhal::Node implementing single I2C Master Slave connection to
 *             control SFP expander chips.
 * @author     Alessandro Thea
 * @date       April 2018
 */
class DACNode : public I2CMasterNode, public DACSlave {
    UHAL_DERIVEDNODE(DACNode)
public:
    DACNode(const uhal::Node& node);
    DACNode(const DACNode& node);
    virtual ~DACNode();

};


} // namespace pdt
} // namespace dunedaq

#endif /* TIMING_BOARD_SOFTWARE_INCLUDE_PDT_DACNODE_HPP_ */