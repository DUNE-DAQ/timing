/**
 * @file I2CMasterNode.hpp
 *
 * I2CMasterNode is a class providing an interface
 * to the I2C master firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_I2CMASTERNODE_HPP_
#define	TIMING_BOARD_SOFTWARE_INCLUDE_PDT_I2CMASTERNODE_HPP_

#include "pdt/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include "TimingIssues.hpp"
#include "ers/ers.h"

namespace dunedaq {
namespace pdt {

// PDTExceptionClass(I2CSlaveNotFound, "Exception class to handle missing I2C slaves");
// PDTExceptionClass(I2CException, "Exception class to handle I2C Exceptions");

class I2CSlave; 

/*!
 * @class      I2CMasterNode
 *
 * @brief      OpenCode I2C interface to a ipbus node
 * @author     Kristian Harder, Alessandro Thea
 * @date       August 2013
 *
 *             The class is non-copyable on purpose as the inheriting object
 *             must properly set the node pointer in the copy i2c access through
 *             an IPbus interface
 */
class I2CMasterNode : public uhal::Node {
    UHAL_DERIVEDNODE(I2CMasterNode)
public:
    I2CMasterNode(const uhal::Node& aNode);
    I2CMasterNode(const I2CMasterNode& aOther );
    virtual ~I2CMasterNode();

    ///
    virtual uint16_t get_i2c_clock_prescale() const {
        return mClockPrescale;
    }

    virtual std::vector<std::string>  get_slaves() const;
    virtual uint8_t get_slave_address( const std::string& name ) const;
    virtual const I2CSlave& get_slave( const std::string& name ) const;

    void reset() const;

    /// commodity functions
    virtual uint8_t read_i2c(uint8_t aSlaveAddress, uint32_t i2cAddress) const;
    virtual void write_i2c(uint8_t aSlaveAddress, uint32_t i2cAddress, uint8_t aData, bool aSendStop = true) const;

    virtual std::vector<uint8_t> read_i2cArray(uint8_t aSlaveAddress, uint32_t i2cAddress, uint32_t aNumWords) const;
    virtual void write_i2cArray(uint8_t aSlaveAddress, uint32_t i2cAddress, std::vector<uint8_t> aData, bool aSendStop = true ) const;

    virtual std::vector<uint8_t> read_i2cPrimitive(uint8_t aSlaveAddress, uint32_t aNumBytes) const;
    virtual void write_i2cPrimitive(uint8_t aSlaveAddress, const std::vector<uint8_t>& aData, bool aSendStop = true) const;

    bool ping(uint8_t aSlaveAddress) const;

    std::vector<uint8_t> scan() const;
protected:

    // low level i2c functions
    std::vector<uint8_t> virtual read_block_i2c(uint8_t aSlaveAddress, uint32_t aNumBytes) const;
    void virtual write_block_i2c(uint8_t aSlaveAddress, const std::vector<uint8_t>& aData, bool aSendStop = true) const;

    uint8_t send_i2c_command_and_read_data( uint8_t aCmd ) const;
    void send_i2c_command_and_write_data( uint8_t aCmd, uint8_t aData ) const;

    //! Slaves 
    std::unordered_map<std::string,uint8_t> mSlavesAddresses;

private:
    ///
    void constructor();
    
    // low level i2c functions
    void wait_until_finished(bool requireAcknowledgement = true, bool requireBusIdleAtEnd = false) const;
    
    //! IPBus register names for i2c bus
    static const std::string kPreHiNode;
    static const std::string kPreLoNode;
    static const std::string kCtrlNode;
    static const std::string kTxNode;
    static const std::string kRxNode;
    static const std::string kCmdNode;
    static const std::string kStatusNode;

    static const uint8_t kStartCmd; // 1 << 7
    static const uint8_t kStopCmd;  // 1 << 6
    static const uint8_t kReadFromSlaveCmd; // 1 << 5
    static const uint8_t kWriteToSlaveCmd; // 1 << 4
    static const uint8_t kAckCmd; // 1 << 3
    static const uint8_t kInterruptAck; // 1


    static const uint8_t kReceivedAckBit;// recvdack = 0x1 << 7
    static const uint8_t kBusyBit;// busy = 0x1 << 6
    static const uint8_t kArbitrationLostBit;// arblost = 0x1 << 5
    static const uint8_t kInProgressBit;// inprogress = 0x1 << 1
    static const uint8_t kInterruptBit;// interrupt = 0x1
    
    //! clock prescale factor
    uint16_t mClockPrescale;

    //! I2C slaves attached to this node
    std::unordered_map<std::string,I2CSlave*> mSlaves;

    friend class I2CSlave;
};

} // namespace pdt
} // namespace dunedaq

#endif	/* TIMING_BOARD_SOFTWARE_INCLUDE_PDT_I2CMASTERNODE_HPP_ */

