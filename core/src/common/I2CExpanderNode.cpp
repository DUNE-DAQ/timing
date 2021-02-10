#include "pdt/I2CExpanderNode.hpp"

// PDT headers
// #include "pdt/toolbox.hpp"
// #include "pdt/Logger.hpp"

// #include <boost/tuple/tuple.hpp>

// #include <fstream>
// #include <sstream>

namespace pdt {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(I2CExpanderNode);

//-----------------------------------------------------------------------------
I2CExpanderSlave::I2CExpanderSlave( const I2CMasterNode* aMaster, uint8_t aAddr ) :
I2CSlave( aMaster, aAddr ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CExpanderSlave::~I2CExpanderSlave( ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CExpanderSlave::ensureValidBankId( uint8_t aBankId ) const {
    if ( aBankId == 0 or aBankId == 1) return;

    std::ostringstream lExc;
    lExc << "Invalid bank id " << aBankId;
    throw SFPExpanderBankIDError(lExc.str());

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CExpanderSlave::setInversion( uint8_t aBankId, uint32_t aInversionMask ) const {

    this->ensureValidBankId(aBankId);
    this->writeI2C(0x4+aBankId, aInversionMask);

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CExpanderSlave::setIO( uint8_t aBankId, uint32_t aIOMask ) const {

    this->ensureValidBankId(aBankId);
    this->writeI2C(0x6+aBankId, aIOMask);

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CExpanderSlave::setOutputs( uint8_t aBankId, uint32_t aValues ) const {

    this->ensureValidBankId(aBankId);
    this->writeI2C(0x2+aBankId, aValues);
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
I2CExpanderSlave::readInputs( uint8_t aBankId ) const {

    this->ensureValidBankId(aBankId);
    return this->readI2C(0x0+aBankId);
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint32_t>
I2CExpanderSlave::debug() const {

    std::vector<uint32_t> lValues(8);

    for ( size_t a(0); a<8; ++a) {
        lValues[a] = this->readI2C(a);
    }
    return lValues;
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
I2CExpanderSlave::readOutputsConfig( uint8_t aBankId ) const {

    this->ensureValidBankId(aBankId);
    return this->readI2C(0x2+aBankId);
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CExpanderNode::I2CExpanderNode( const uhal::Node& aNode ) : I2CMasterNode(aNode), I2CExpanderSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CExpanderNode::I2CExpanderNode( const I2CExpanderNode& aOther ) : I2CMasterNode(aOther), I2CExpanderSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CExpanderNode::~I2CExpanderNode() {
}
//-----------------------------------------------------------------------------

} // namespace pdt
