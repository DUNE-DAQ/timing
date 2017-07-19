/**
 * @file    I2CMasterNode.hpp
 * @author  Alessandro Thea
 * @brief   Brief description
 * @date 
 */

#ifndef __PDT_I2CBASENODE_HPP__
#define	__PDT_I2CBASENODE_HPP__

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include "pdt/exception.hpp"
 
namespace pdt {

// PDTExceptionClass(I2CSlaveNotFound, "Exception class to handle missing I2C slaves");
// PDTExceptionClass(I2CException, "Exception class to handle I2C Exceptions");

class I2CSlave; 

/*!
 * @class      I2CBaseNode
 *
 * @brief      OpenCode I2C interface to a ipbus node
 * @author     Kristian Harder, Alessandro Thea
 * @date       August 2013
 *
 *             The class is non-copyable on purpose as the inheriting object
 *             must properly set the node pointer in the copy i2c access through
 *             an IPbus interface
 */
class I2CBaseNode : public uhal::Node {
    UHAL_DERIVEDNODE(I2CBaseNode);
public:
    I2CBaseNode(const uhal::Node& aNode);
//    I2CBaseNode(const I2CBaseNode& aOther);
    virtual ~I2CBaseNode();

    ///
    virtual uint8_t getI2CClockPrescale() const {
        return mClockPrescale;
    }

    virtual std::vector<std::string>  getSlaves() const;
    virtual uint8_t  getSlaveAddress( const std::string& name ) const;
    /// commodity functions
    virtual uint8_t readI2C(uint8_t aSlaveAddress, uint32_t i2cAddress) const;
    virtual void writeI2C(uint8_t aSlaveAddress, uint32_t i2cAddress, uint32_t data) const;
    
protected:
    // low level i2c functions
    std::vector<uint8_t> virtual readBlockI2C(uint8_t aSlaveAddress, uint32_t numBytes) const;
    void virtual writeBlockI2C(uint8_t aSlaveAddress, const std::vector<uint8_t>& data) const;

    //! Slaves 
    boost::unordered_map<std::string,uint8_t> mSlavesAddresses;

private:
    ///
    void constructor();
    
    // low level i2c functions
    void reset() const;
    void waitUntilFinished(bool requireAcknowledgement = true,
            bool requireBusIdleAtEnd = false) const;
    
    //! IPBus register names for i2c bus
    static const std::string kPreHi;
    static const std::string kPreLo;
    static const std::string kCtrl;
    static const std::string kTx;
    static const std::string kRx;
    static const std::string kCmd;
    static const std::string kStatus;

    static const uint32_t kStartCmd; // 1 << 7
    static const uint32_t kStopCmd;  // 1 << 6
    static const uint32_t kReadFromSlaveCmd; // 1 << 5
    static const uint32_t kWriteToSlaveCmd; // 1 << 4
    static const uint32_t kAckCmd; // 1 << 3
    static const uint32_t kInterruptAck; // 1

    //! clock prescale factor
    uint16_t mClockPrescale;

    friend class I2CSlave;
};

} // namespace pdt

#endif	/* __PDT_I2CBASENODE_HPP__ */

