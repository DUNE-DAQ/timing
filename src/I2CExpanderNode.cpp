#include "pdt/I2CExpanderNode.hpp"

// PDT headers
#include "pdt/toolbox.hpp"
#include "ers/ers.h"

// #include <boost/tuple/tuple.hpp>

// #include <fstream>
// #include <sstream>

namespace dunedaq {
namespace pdt {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(I2CExpanderNode)

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

    throw SFPExpanderBankIDError(ERS_HERE, "I2CExpanderSlave", std::to_string(aBankId));
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CExpanderSlave::set_inversion( uint8_t aBankId, uint32_t aInversionMask ) const {

    this->ensureValidBankId(aBankId);
    this->write_i2c(0x4+aBankId, aInversionMask);

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CExpanderSlave::set_io( uint8_t aBankId, uint32_t aIOMask ) const {

    this->ensureValidBankId(aBankId);
    this->write_i2c(0x6+aBankId, aIOMask);

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CExpanderSlave::set_outputs( uint8_t aBankId, uint32_t aValues ) const {

    this->ensureValidBankId(aBankId);
    this->write_i2c(0x2+aBankId, aValues);
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
I2CExpanderSlave::read_inputs( uint8_t aBankId ) const {

    this->ensureValidBankId(aBankId);
    return this->read_i2c(0x0+aBankId);
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint32_t>
I2CExpanderSlave::debug() const {

    std::vector<uint32_t> lValues(8);

    for ( size_t a(0); a<8; ++a) {
        lValues[a] = this->read_i2c(a);
    }
    return lValues;
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CExpanderNode::I2CExpanderNode( const uhal::Node& aNode ) : I2CMasterNode(aNode), I2CExpanderSlave(this, this->get_slave_address("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CExpanderNode::I2CExpanderNode( const I2CExpanderNode& aOther ) : I2CMasterNode(aOther), I2CExpanderSlave(this, this->get_slave_address("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CExpanderNode::~I2CExpanderNode() {
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq