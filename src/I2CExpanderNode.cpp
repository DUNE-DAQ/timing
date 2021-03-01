#include "pdt/I2CExpanderNode.hpp"

// PDT headers
#include "pdt/toolbox.hpp"
#include "ers/ers.hpp"

// #include <boost/tuple/tuple.hpp>

// #include <fstream>
// #include <sstream>

namespace dunedaq {
namespace pdt {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(I2CExpanderNode)

//-----------------------------------------------------------------------------
I2CExpanderSlave::I2CExpanderSlave( const I2CMasterNode* i2c_master, uint8_t address ) :
I2CSlave( i2c_master, address ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CExpanderSlave::~I2CExpanderSlave( ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CExpanderSlave::ensure_valid_bank_id( uint8_t bank_id ) const {
    if ( bank_id == 0 or bank_id == 1) return;

    throw SFPExpanderBankIDError(ERS_HERE, "I2CExpanderSlave", std::to_string(bank_id));
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CExpanderSlave::set_inversion( uint8_t bank_id, uint32_t inversion_mask ) const {

    this->ensure_valid_bank_id(bank_id);
    this->write_i2c(0x4+bank_id, inversion_mask);

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CExpanderSlave::set_io( uint8_t bank_id, uint32_t io_mask ) const {

    this->ensure_valid_bank_id(bank_id);
    this->write_i2c(0x6+bank_id, io_mask);

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CExpanderSlave::set_outputs( uint8_t bank_id, uint32_t output_values ) const {

    this->ensure_valid_bank_id(bank_id);
    this->write_i2c(0x2+bank_id, output_values);
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
I2CExpanderSlave::read_inputs( uint8_t bank_id ) const {

    this->ensure_valid_bank_id(bank_id);
    return this->read_i2c(0x0+bank_id);
    
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
I2CExpanderNode::I2CExpanderNode( const uhal::Node& node ) : I2CMasterNode(node), I2CExpanderSlave(this, this->get_slave_address("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CExpanderNode::I2CExpanderNode( const I2CExpanderNode& node ) : I2CMasterNode(node), I2CExpanderSlave(this, this->get_slave_address("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CExpanderNode::~I2CExpanderNode() {
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq