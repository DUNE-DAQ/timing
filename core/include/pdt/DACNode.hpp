#ifndef __PDT_DACNODE_HPP__
#define __PDT_DACNODE_HPP__

#include "pdt/I2CSlave.hpp"
#include "pdt/I2CMasterNode.hpp"

namespace pdt {


PDTExceptionClass(DACChannelOutOfRange, "DAC channel out of range");
PDTExceptionClass(DACValueOutOfRange, "DAC channel out of range");


/**
 * @brief      Class for dac node.
 */
class DACSlave : public I2CSlave
{
public:
    DACSlave(const I2CMasterNode* aMaster, uint8_t aSlaveAddress);
    virtual ~DACSlave() = default;
  
    void setInteralRef( bool aInternal ) const;

    void setDAC(uint8_t aChan, uint32_t aCode) const;
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
    UHAL_DERIVEDNODE(DACNode);
public:
    DACNode(const uhal::Node& aNode);
    DACNode(const DACNode& aOther);
    virtual ~DACNode();

};


} // namespace pdt

#endif /* __PDT_DACNODE_HPP__ */