/* 
 * File:   OpenCoresI2CMasterNode.cpp
 * Author: ale
 * 
 * Created on August 29, 2014, 4:47 PM
 */

#include "pdt/I2CSlave.hpp"


#include <boost/lexical_cast.hpp>
#include <boost/unordered/unordered_map.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "pdt/exception.hpp"
#include "pdt/Logger.hpp"
#include "pdt/toolbox.hpp"
#include "pdt/I2CBaseNode.hpp"

namespace pdt {


//-----------------------------------------------------------------------------
I2CSlave::I2CSlave(const I2CBaseNode* aMaster, uint8_t aSlaveAddress) :
mMaster(aMaster), mAddress(aSlaveAddress) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CSlave::~I2CSlave() {

}
//-----------------------------------------------------------------------------


// comodity functions
//-----------------------------------------------------------------------------
uint8_t
I2CSlave::readI2C(uint32_t i2cAddress) const {
    return mMaster->readI2C(mAddress, i2cAddress);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CSlave::writeI2C(uint32_t i2cAddress, uint8_t aData) const {
    mMaster->writeI2C(mAddress, i2cAddress, aData);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint8_t>
I2CSlave::readI2CArray(uint32_t i2cAddress, uint32_t aNumWords) const {
    return mMaster->readI2CArray(mAddress, i2cAddress, aNumWords);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CSlave::writeI2CArray(uint32_t i2cAddress, std::vector<uint8_t> aData) const {
    mMaster->writeI2CArray(mAddress, i2cAddress, aData);
}
//-----------------------------------------------------------------------------

} // namespace pdt

