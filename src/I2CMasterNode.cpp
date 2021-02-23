/* 
 * File:   OpenCoresI2CMasterNode.cpp
 * Author: ale
 * 
 * Created on August 29, 2014, 4:47 PM
 */

#include "pdt/I2CMasterNode.hpp"


#include <boost/lexical_cast.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "pdt/TimingIssues.hpp"
#include "ers/ers.h"
#include "pdt/toolbox.hpp"
#include "pdt/I2CSlave.hpp"

namespace dunedaq {
namespace pdt {

UHAL_REGISTER_DERIVED_NODE(I2CMasterNode)

// PRIVATE CONST definitions
const std::string I2CMasterNode::kPreHiNode = "ps_hi";
const std::string I2CMasterNode::kPreLoNode = "ps_lo";
const std::string I2CMasterNode::kCtrlNode = "ctrl";
const std::string I2CMasterNode::kTxNode = "data";
const std::string I2CMasterNode::kRxNode = "data";
const std::string I2CMasterNode::kCmdNode = "cmd_stat";
const std::string I2CMasterNode::kStatusNode = "cmd_stat";

const uint8_t I2CMasterNode::kStartCmd = 0x80; // 1 << 7
const uint8_t I2CMasterNode::kStopCmd = 0x40;  // 1 << 6
const uint8_t I2CMasterNode::kReadFromSlaveCmd = 0x20; // 1 << 5
const uint8_t I2CMasterNode::kWriteToSlaveCmd = 0x10; // 1 << 4
const uint8_t I2CMasterNode::kAckCmd = 0x08; // 1 << 3
const uint8_t I2CMasterNode::kInterruptAck = 0x01; // 1

const uint8_t I2CMasterNode::kReceivedAckBit = 0x80;// recvdack = 0x1 << 7
const uint8_t I2CMasterNode::kBusyBit = 0x40;// busy = 0x1 << 6
const uint8_t I2CMasterNode::kArbitrationLostBit = 0x20;// arblost = 0x1 << 5
const uint8_t I2CMasterNode::kInProgressBit = 0x2;// inprogress = 0x1 << 1
const uint8_t I2CMasterNode::kInterruptBit = 0x1;// interrupt = 0x1

//-----------------------------------------------------------------------------
I2CMasterNode::I2CMasterNode(const uhal::Node& node) : uhal::Node(node) {
    constructor();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CMasterNode::I2CMasterNode(const I2CMasterNode& node) : uhal::Node(node) {
    constructor();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void I2CMasterNode::constructor() {
    // 16 bit clock prescale factor.
    // formula: m_clockPrescale = (input_frequency / 5 / desired_frequency) -1
    // for typical IPbus applications: input frequency = IPbus clock = 31.x MHz
    // target frequency 100 kHz to play it safe (first revision of i2c standard),
    m_clock_prescale = 0x40;
    // m_clock_prescale = 0x100;
    
    // Build the list of slaves
    // Loop over node parameters. Each parameter becomes a slave node.
    const std::unordered_map<std::string, std::string>& lPars = this->getParameters();
    std::unordered_map<std::string, std::string>::const_iterator lIt;
    for ( lIt = lPars.begin(); lIt != lPars.end(); ++lIt ) {
        uint32_t lSlaveAddr = (boost::lexical_cast< pdt::stoul<uint32_t> > (lIt->second) & 0x7f);
        m_i2c_device_addresses.insert(std::make_pair( lIt->first, lSlaveAddr  ) );
        m_i2c_devices.insert(std::make_pair( lIt->first, new I2CSlave( this, lSlaveAddr ) ) );
    }
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CMasterNode::~I2CMasterNode() {
    std::unordered_map<std::string, I2CSlave*>::iterator lIt;
    for ( lIt = m_i2c_devices.begin(); lIt != m_i2c_devices.end(); ++lIt ) {
        // Delete slaves
        delete lIt->second;
    }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<std::string>
I2CMasterNode::get_slaves() const {
    std::vector<std::string> lSlaves;

    boost::copy(m_i2c_device_addresses | boost::adaptors::map_keys, std::back_inserter(lSlaves));
    return lSlaves;
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
I2CMasterNode::get_slave_address(const std::string& name) const {
    std::unordered_map<std::string, uint8_t>::const_iterator lIt = m_i2c_device_addresses.find(name);
    if ( lIt == m_i2c_device_addresses.end() ) {
        throw I2CDeviceNotFound(ERS_HERE, getId(), getId(), name);
    }
    return lIt->second;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
const I2CSlave&
I2CMasterNode::get_slave(const std::string& name) const {
    std::unordered_map<std::string, I2CSlave*>::const_iterator lIt = m_i2c_devices.find(name);
    if ( lIt == m_i2c_devices.end() ) {
        throw I2CDeviceNotFound(ERS_HERE, getId(), getId(), name);
    }
    return *(lIt->second);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
I2CMasterNode::read_i2c(uint8_t i2c_device_address, uint32_t i2c_reg_address) const {
    // // write one word containing the address
    // std::vector<uint8_t> array(1, i2c_reg_address & 0x7f);
    // this->write_block_i2c(i2c_device_address, array);
    // // request the content at the specific address
    // return this->read_block_i2c(i2c_device_address, 1) [0];
    return this->read_i2cArray(i2c_device_address, i2c_reg_address, 1)[0];
}
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
void
I2CMasterNode::write_i2c(uint8_t i2c_device_address, uint32_t i2c_reg_address, uint8_t data, bool send_stop) const {
    // std::vector<uint8_t> block(2);
    // block[0] = (i2c_reg_address & 0xff);
    // block[1] = (data & 0xff);
    // this->write_block_i2c(i2c_device_address, block);

    this->write_i2cArray( i2c_device_address, i2c_reg_address, {data}, send_stop);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint8_t>
I2CMasterNode::read_i2cArray(uint8_t i2c_device_address, uint32_t i2c_reg_address, uint32_t number_of_words) const {
    // write one word containing the address
    std::vector<uint8_t> lArray{(uint8_t)(i2c_reg_address & 0xff)};
    this->write_block_i2c(i2c_device_address, lArray);
    // request the content at the specific address
    return this->read_block_i2c(i2c_device_address, number_of_words);
}
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
void
I2CMasterNode::write_i2cArray(uint8_t i2c_device_address, uint32_t i2c_reg_address, std::vector<uint8_t> data, bool send_stop) const {
    // std::cout << "Writing " << data.size() << " from " << std::showbase <<  std::hex << i2c_reg_address << " on " << (uint32_t)i2c_device_address << std::endl; // HACK
    std::vector<uint8_t> block(data.size() + 1);
    block[0] = (i2c_reg_address & 0xff);

    for ( size_t i(0); i < data.size(); ++i )
        block[i+1] = data[i];

    this->write_block_i2c(i2c_device_address, block, send_stop);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint8_t> 
I2CMasterNode::read_i2cPrimitive(uint8_t i2c_device_address, uint32_t number_of_bytes) const {
    return this->read_block_i2c(i2c_device_address, number_of_bytes); 
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CMasterNode::write_i2cPrimitive(uint8_t i2c_device_address, const std::vector<uint8_t>& data, bool send_stop) const {
    this->write_block_i2c( i2c_device_address, data, send_stop );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CMasterNode::write_block_i2c(uint8_t i2c_device_address, const std::vector<uint8_t>& aArray, bool send_stop) const {
    // transmit reg definitions
    // bits 7-1: 7-bit slave address during address transfer
    //           or first 7 bits of byte during data transfer
    // bit 0: RW flag during address transfer or LSB during data transfer.
    // '1' = reading from slave
    // '0' = writing to slave
    // command reg definitions
    // bit 7: Generate start condition
    // bit 6: Generate stop condition
    // bit 5: Read from slave
    // bit 4: Write to slave
    // bit 3: 0 when acknowledgement is received
    // bit 2:1: Reserved
    // bit 0: Interrupt acknowledge. When set, clears a pending interrupt
    
    // Reset bus before beginning
    reset();

    // Open the connection and send the slave address, bit 0 set to zero
    send_i2c_command_and_write_data(kStartCmd, (i2c_device_address << 1) & 0xfe);

    for (unsigned iByte = 0; iByte < aArray.size(); iByte++) {

        // Send stop if last element of the array (and not vetoed)
        uint8_t lCmd = ( ((iByte == aArray.size() - 1) && send_stop) ? kStopCmd : 0x0);

        // Push the byte on the bus
        send_i2c_command_and_write_data(lCmd, aArray[iByte]);
    }

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint8_t>
I2CMasterNode::read_block_i2c(uint8_t i2c_device_address, uint32_t lNumBytes) const {
    // transmit reg definitions
    // bits 7-1: 7-bit slave address during address transfer
    //           or first 7 bits of byte during data transfer
    // bit 0: RW flag during address transfer or LSB during data transfer.
    //        '1' = reading from slave
    //        '0' = writing to slave
    // command reg definitions
    // bit 7:   Generate start condition
    // bit 6:   Generate stop condition
    // bit 5:   Read from slave
    // bit 4:   Write to slave
    // bit 3:   0 when acknowledgement is received
    // bit 2:1: Reserved
    // bit 0:   Interrupt acknowledge. When set, clears a pending interrupt
    
    // Reset bus before beginning
    reset();

    // Open the connection & send the target i2c address. Bit 0 set to 1 (read)
    send_i2c_command_and_write_data(kStartCmd, (i2c_device_address << 1) | 0x01);

    std::vector<uint8_t> lArray;
    for (unsigned iByte = 0; iByte < lNumBytes; iByte++) {

        uint8_t lCmd = (  (iByte == lNumBytes - 1) ? (kStopCmd | kAckCmd) : 0x0); 

        // Push the cmd on the bus, retrieve the result and put it in the arrary
        lArray.push_back( send_i2c_command_and_read_data(lCmd) );
    }
    return lArray;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CMasterNode::ping(uint8_t i2c_device_address) const {

    std::vector<uint8_t> lAddrVector;

    // Reset bus before beginning
    reset();

    try {
        send_i2c_command_and_write_data(kStartCmd, (i2c_device_address << 1) | 0x01);
        send_i2c_command_and_read_data(kStopCmd | kAckCmd);
        return true;
    } catch (const pdt::I2CException& lExc) {
        // PDT_LOG(kError) << std::showbase << std::hex << (uint32_t)iAddr << "  " << lExc.what();
        return false;
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint8_t> 
I2CMasterNode::scan() const {

    std::vector<uint8_t> lAddrVector;

    // Reset bus before beginning
    reset();

    for( uint8_t iAddr(0); iAddr<0x7f; ++iAddr) {
        // Open the connection & send the target i2c address. Bit 0 set to 1 (read)
        try {
            send_i2c_command_and_write_data(kStartCmd, (iAddr << 1) | 0x01);
            send_i2c_command_and_read_data(kStopCmd | kAckCmd);
        } catch (const pdt::I2CException& lExc) {
            // PDT_LOG(kError) << std::showbase << std::hex << (uint32_t)iAddr << "  " << lExc.what();
            continue;
        }
        lAddrVector.push_back(iAddr);
        // PDT_LOG(kNotice) << std::showbase << std::hex << (uint32_t)iAddr << " found";
    }

    return lAddrVector;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void 
I2CMasterNode::reset() const {
    // Resets the I2C bus
    //
    // This function does the following:
    //        1) Disables the I2C core
    //        2) Sets the clock prescale registers
    //        3) Enables the I2C core
    //        4) Sets all writable bus-master registers to default values

    auto ctrl = getNode(kCtrlNode).read();
    auto preHi = getNode(kPreHiNode).read();
    auto preLo = getNode(kPreLoNode).read();
    getClient().dispatch();
    
    bool lFullReset(false);

    lFullReset = (m_clock_prescale != (preHi << 8) + preLo);

    if ( lFullReset ) {
        // disable the I2C core
        getNode(kCtrlNode).write(0x00);
        getClient().dispatch();
        // set the clock prescale
        getNode(kPreHiNode).write((m_clock_prescale & 0xff00) >> 8);
        // getClient().dispatch();
        getNode(kPreLoNode).write(m_clock_prescale & 0xff);
        // getClient().dispatch();
        // set all writable bus-master registers to default values
        getNode(kTxNode).write(0x00);
        getNode(kCmdNode).write(0x00);
        getClient().dispatch();

        // enable the I2C core
        getNode(kCtrlNode).write(0x80);
        getClient().dispatch();
    } else {
        // set all writable bus-master registers to default values
        getNode(kTxNode).write(0x00);
        getNode(kCmdNode).write(0x00);
        getClient().dispatch();
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t 
I2CMasterNode::send_i2c_command_and_read_data( uint8_t command ) const  {

    assert( !(command & kWriteToSlaveCmd) );

    uint8_t lFullCmd = command | kReadFromSlaveCmd;
    ERS_DEBUG(1, ">> sending read cmd  = " << format_reg_value((uint32_t)lFullCmd));


    // Force the read bit high and set them cmd bits
    getNode(kCmdNode).write( lFullCmd );
    getClient().dispatch();

    // Wait for transaction to finish. Require idle bus at the end if stop bit is high)
    wait_until_finished(/*req ack*/ false, command & kStopCmd);

    // Pull the data out of the rx register.
    uhal::ValWord<uint32_t> lResult = getNode(kRxNode).read();
    getClient().dispatch();

    ERS_DEBUG(1, "<< receive data      = " << format_reg_value((uint32_t)lResult));

    return (lResult & 0xff);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CMasterNode::send_i2c_command_and_write_data( uint8_t command, uint8_t data ) const  {

    // 
    assert( !(command & kReadFromSlaveCmd) );
    
    uint8_t lFullCmd = command | kWriteToSlaveCmd;
    std::stringstream debug_stream;
    debug_stream << ">> sending write cmd = " << std::showbase << std::hex << (uint32_t)lFullCmd << " data = " << std::showbase << std::hex << (uint32_t)data;
    ERS_DEBUG(1, debug_stream.str());

    // write the payload
    getNode(kTxNode).write( data );
    getClient().dispatch();

    // Force the write bit high and set them cmd bits
    getNode(kCmdNode).write( lFullCmd );

    // Run the commands and wait for transaction to finish
    getClient().dispatch();

    // Wait for transaction to finish. Require idle bus at the end if stop bit is high
    wait_until_finished(/*req hack*/ true, /*requ idle*/ command & kStopCmd);
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
void I2CMasterNode::wait_until_finished(bool aRequireAcknowledgement, bool aRequireBusIdleAtEnd) const {
    // Ensures the current bus transaction has finished successfully
    // before allowing further I2C bus transactions
    // This method monitors the status register
    // and will not allow execution to continue until the
    // I2C bus has completed properly.  It will throw an exception
    // if it picks up bus problems or a bus timeout occurs.
    const unsigned lMaxRetry = 20;
    unsigned lAttempt = 1;
    bool lReceivedAcknowledge, lBusy;

    const uhal::Node& lStatusNode = getNode(kStatusNode);

    while (lAttempt <= lMaxRetry) {
        usleep(10);
        // Get the status
        uhal::ValWord<uint32_t> i2c_status = lStatusNode.read();
        getClient().dispatch();

        lReceivedAcknowledge = !(i2c_status & kReceivedAckBit);
        lBusy = (i2c_status & kBusyBit);
        bool arbitrationLost = (i2c_status & kArbitrationLostBit);
        bool transferInProgress = (i2c_status & kInProgressBit);

        if (arbitrationLost) {
            // This is an instant error at any time
            throw I2CBusArbitrationLost(ERS_HERE, getId(), getId());
        }

        if (!transferInProgress) {
            // The transfer looks to have completed successfully,
            // pending further checks
            break;
        }

        lAttempt += 1;
    }

    // At this point, we've either had too many retries, or the
    // Transfer in Progress (TIP) bit went low.  If the TIP bit
    // did go low, then we do a couple of other checks to see if
    // the bus operated as expected:

    if (lAttempt > lMaxRetry) {
        throw I2CTransactionTimeout(ERS_HERE, getId(), getId());
    }

    if (aRequireAcknowledgement && !lReceivedAcknowledge) {
        throw I2CNoAcknowledgeReceived(ERS_HERE, getId(), getId());
    }

    if (aRequireBusIdleAtEnd && lBusy) {
        throw I2CTransferFinishedBusStillBusy(ERS_HERE, getId(), getId());
    }
}

} // namespace pdt
} // namespace dunedaq