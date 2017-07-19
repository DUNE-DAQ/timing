#include "pdt/SI5344.hpp"

namespace pdt {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(SI5344Node);

//-----------------------------------------------------------------------------
SI5344Slave::SI5344Slave( const I2CBaseNode* aMaster, uint8_t aAddr ) :
I2CSlave( aMaster, aAddr ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI5344Slave::~SI5344Slave( ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::map<uint32_t, uint32_t>
SI5344Slave::registers( ) const {
    //  boost::format fmthex("%d");
    std::map<uint32_t, uint32_t> values;

    // for( uint8_t regAddr = 0; regAddr <= 143; regAddr++ ) {
    //     if( regAddr > 11 && regAddr < 16 ) {
    //         continue;
    //     }

    //     if( regAddr > 25 && regAddr < 31 ) {
    //         continue;
    //     }

    //     if( regAddr > 36 && regAddr < 40 ) {
    //         continue;
    //     }

    //     if( regAddr > 48 && regAddr < 55 ) {
    //         continue;
    //     }

    //     if( regAddr > 55 && regAddr < 128 ) {
    //         continue;
    //     }

    //     if( regAddr == 133 ) {
    //         continue;
    //     }

    //     if( regAddr == 137 ) {
    //         continue;
    //     }

    //     if( regAddr > 139 && regAddr < 142 ) {
    //         continue;
    //     }

    //     values[ regAddr ] = readI2C(regAddr);
    // }

    return values;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SI5344Node::SI5344Node( const uhal::Node& aNode ) : I2CBaseNode(aNode), SI5344Slave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI5344Node::SI5344Node( const SI5344Node& aOther ) : I2CBaseNode(aOther), SI5344Slave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI5344Node::~SI5344Node() {
}
//-----------------------------------------------------------------------------



} // namespace pdt