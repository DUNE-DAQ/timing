#include "pdt/DACNode.hpp"

namespace pdt {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(DACNode)

//-----------------------------------------------------------------------------
DACSlave::DACSlave( const I2CMasterNode* aMaster, uint8_t aAddr ) :
    I2CSlave( aMaster, aAddr ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
DACSlave::setInteralRef( bool aInternal ) const {
    this->writeI2CArray(0x38, {0x0, aInternal});
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
DACSlave::setDAC(uint8_t aChan, uint32_t aCode) const {

    if (aChan>7) {
        throw DACChannelOutOfRange(ERS_HERE, "DACSlave", std::to_string(aChan));
    }

    if ( aCode > 0xffff ) {
        throw DACValueOutOfRange(ERS_HERE, "DACSlave", std::to_string(aCode));
    }

    uint32_t lAddr = 0x18 + (aChan & 0x7);

    this->writeI2CArray(lAddr, {(uint8_t)((aCode >> 8) & 0xff), (uint8_t)(aCode & 0xff)});
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
DACNode::DACNode( const uhal::Node& aNode ) : I2CMasterNode(aNode), DACSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
DACNode::DACNode( const DACNode& aOther ) : I2CMasterNode(aOther), DACSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
DACNode::~DACNode() {
}
//-----------------------------------------------------------------------------

} // namespace pdt