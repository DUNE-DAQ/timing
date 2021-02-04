/**
 * @file    I2CMasterNode.hpp
 * @author  Alessandro Thea
 * @brief   Brief description
 * @date 
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_I2CSLAVE_HPP_
#define	TIMING_BOARD_SOFTWARE_INCLUDE_PDT_I2CSLAVE_HPP_

#include "ers/ers.h"

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include "TimingIssues.hpp"

#include <boost/core/noncopyable.hpp>
#include <vector>

namespace pdt {

class I2CMasterNode;

/*!
 * @class I2CSlave
 * @brief Class to provide OpenCode I2C interface to a ipbus node
 *
 * The class is non copyable on purpose as the inheriting object
 * must properly set the node pointer in the copy
 * i2c access through an IPbus interface
 * @author Kristian Harder, Alessandro Thea
 * @date August 2013
 *
 */
class I2CSlave : boost::noncopyable {
protected:
    // Private constructor, accessible to I2CMaster
    I2CSlave(const I2CMasterNode* aMaster, uint8_t aSlaveAddress);
public:

    virtual ~I2CSlave();

    ///

    uint8_t getI2CAddress() const {
        return mAddress;
    }

    /// comodity functions
    uint8_t readI2C(uint32_t aDeviceAddress, uint32_t aRegAddress) const;
    uint8_t readI2C(uint32_t i2cAddress) const;

    void writeI2C(uint32_t aDeviceAddress, uint32_t i2cAddress, uint8_t aData, bool aSendStop = true) const;
    void writeI2C(uint32_t i2cAddress, uint8_t aData, bool aSendStop = true) const;

    std::vector<uint8_t> readI2CArray(uint32_t aDeviceAddress, uint32_t aRegAddress, uint32_t aNumWords) const;
    std::vector<uint8_t> readI2CArray(uint32_t i2cAddress, uint32_t aNumWords) const;
    
    void writeI2CArray(uint32_t aDeviceAddress, uint32_t i2cAddress, std::vector<uint8_t> aData, bool aSendStop = true) const;
    void writeI2CArray(uint32_t i2cAddress, std::vector<uint8_t> aData, bool aSendStop = true) const;


    std::vector<uint8_t> readI2CPrimitive(uint32_t aNumBytes) const;
    void writeI2CPrimitive(const std::vector<uint8_t>& aData, bool aSendStop = true) const;

    bool ping() const;

    std::string getMasterId() const;

private:
    const I2CMasterNode* mMaster;

    // slave address
    uint8_t mAddress;

    friend class I2CMasterNode;
};

} // namespace pdt

#endif	/* TIMING_BOARD_SOFTWARE_INCLUDE_PDT_I2CSLAVE_HPP_ */

