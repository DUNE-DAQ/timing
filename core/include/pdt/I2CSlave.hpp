/**
 * @file    I2CMasterNode.hpp
 * @author  Alessandro Thea
 * @brief   Brief description
 * @date 
 */

#ifndef __PDT_I2CSLAVE_HPP__
#define	__PDT_I2CSLAVE_HPP__

#include <vector>
 
// uHal Headers
#include "uhal/DerivedNode.hpp"
#include "pdt/exception.hpp"
 
namespace pdt {

class I2CBaseNode;

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
    I2CSlave(const I2CBaseNode* aMaster, uint8_t aSlaveAddress);
public:

    virtual ~I2CSlave();

    ///

    uint8_t getI2CAddress() const {
        return mAddress;
    }

    /// comodity functions
    uint8_t readI2C(uint32_t i2cAddress) const;
    void writeI2C(uint32_t i2cAddress, uint8_t aData) const;

    std::vector<uint8_t> readI2CArray(uint32_t i2cAddress, uint32_t aNumWords) const;
    void writeI2CArray(uint32_t i2cAddress, std::vector<uint8_t> aData) const;

private:
    const I2CBaseNode* mMaster;

    // slave address
    uint8_t mAddress;

    friend class I2CMasterNode;
};

} // namespace pdt

#endif	/* __PDT_I2CSLAVE_HPP__ */

