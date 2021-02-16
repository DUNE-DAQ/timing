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

#include "pdt/TimingIssues.hpp"
#include "ers/ers.h"
#include "pdt/toolbox.hpp"
#include "pdt/I2CMasterNode.hpp"

namespace dunedaq {
namespace pdt {


//-----------------------------------------------------------------------------
I2CSlave::I2CSlave(const I2CMasterNode* aMaster, uint8_t aSlaveAddress) :
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
I2CSlave::read_i2c(uint32_t aDeviceAddress, uint32_t aRegAddress) const {
	return mMaster->read_i2c(aDeviceAddress, aRegAddress);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
I2CSlave::read_i2c(uint32_t i2cAddress) const {
    return mMaster->read_i2c(mAddress, i2cAddress);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CSlave::write_i2c(uint32_t aDeviceAddress, uint32_t i2cAddress, uint8_t aData, bool aSendStop) const {
    mMaster->write_i2c(aDeviceAddress, i2cAddress, aData, aSendStop);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CSlave::write_i2c(uint32_t i2cAddress, uint8_t aData, bool aSendStop) const {
    mMaster->write_i2c(mAddress, i2cAddress, aData, aSendStop);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint8_t>
I2CSlave::read_i2cArray(uint32_t aDeviceAddress, uint32_t aRegAddress, uint32_t aNumWords) const {
	return mMaster->read_i2cArray(aDeviceAddress, aRegAddress, aNumWords);	
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint8_t>
I2CSlave::read_i2cArray(uint32_t i2cAddress, uint32_t aNumWords) const {
    return mMaster->read_i2cArray(mAddress, i2cAddress, aNumWords);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CSlave::write_i2cArray(uint32_t aDeviceAddress,uint32_t i2cAddress, std::vector<uint8_t> aData, bool aSendStop) const {
    mMaster->write_i2cArray(aDeviceAddress, i2cAddress, aData, aSendStop);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CSlave::write_i2cArray(uint32_t i2cAddress, std::vector<uint8_t> aData, bool aSendStop) const {
    mMaster->write_i2cArray(mAddress, i2cAddress, aData, aSendStop);
}
//-----------------------------------------------------------------------------

// comodity functions
//-----------------------------------------------------------------------------
std::vector<uint8_t>
I2CSlave::read_i2cPrimitive(uint32_t aNumBytes) const {
    return mMaster->read_i2cPrimitive(mAddress, aNumBytes);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CSlave::write_i2cPrimitive(const std::vector<uint8_t>& aData, bool aSendStop) const {
    mMaster->write_i2cPrimitive(mAddress, aData, aSendStop);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CSlave::ping() const {
    return mMaster->ping(mAddress);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
I2CSlave::get_master_id() const {
    return mMaster->getId();
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq