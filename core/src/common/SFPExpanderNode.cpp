#include "pdt/SFPExpanderNode.hpp"

// PDT headers
// #include "pdt/toolbox.hpp"
// #include "pdt/Logger.hpp"

// #include <boost/tuple/tuple.hpp>

// #include <fstream>
// #include <sstream>

namespace pdt {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(SFPExpanderNode);

//-----------------------------------------------------------------------------
SFPExpanderSlave::SFPExpanderSlave( const I2CBaseNode* aMaster, uint8_t aAddr ) :
I2CSlave( aMaster, aAddr ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SFPExpanderSlave::~SFPExpanderSlave( ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SFPExpanderSlave::ensureValidBankId( uint8_t aBankId ) const {
    if ( aBankId == 0 or aBankId == 1) return;

    std::ostringstream lExc;
    lExc << "Invalid bank id " << aBankId;
    throw SFPExpanderBankIDError(lExc.str());

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SFPExpanderSlave::setInversion( uint8_t aBankId, uint32_t aInversionMask ) const {

    this->ensureValidBankId(aBankId);
    this->writeI2C(0x4+aBankId, aInversionMask);

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SFPExpanderSlave::setIO( uint8_t aBankId, uint32_t aIOMask ) const {

    this->ensureValidBankId(aBankId);
    this->writeI2C(0x6+aBankId, aIOMask);

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SFPExpanderSlave::enable( uint8_t aBankId, uint32_t aEnableMask ) const {

    this->ensureValidBankId(aBankId);
    this->writeI2C(0x2+aBankId, aEnableMask);

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SFPExpanderSlave::writeValues( uint8_t aBankId, uint32_t aValues ) const {

    this->ensureValidBankId(aBankId);
    this->writeI2C(0x0+aBankId, aValues);
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
SFPExpanderSlave::readValues( uint8_t aBankId ) const {

    this->ensureValidBankId(aBankId);
    return this->readI2C(0x0+aBankId);
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint32_t>
SFPExpanderSlave::debug() const {

    std::vector<uint32_t> lValues(8);

    for ( size_t a(0); a<8; ++a) {
        lValues[a] = this->readI2C(a);
    }
    return lValues;
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SFPExpanderNode::SFPExpanderNode( const uhal::Node& aNode ) : I2CBaseNode(aNode), SFPExpanderSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SFPExpanderNode::SFPExpanderNode( const SFPExpanderNode& aOther ) : I2CBaseNode(aOther), SFPExpanderSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SFPExpanderNode::~SFPExpanderNode() {
}
//-----------------------------------------------------------------------------

}