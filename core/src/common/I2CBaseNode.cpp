/* 
 * File:   OpenCoresI2CMasterNode.cpp
 * Author: ale
 * 
 * Created on August 29, 2014, 4:47 PM
 */

#include "pdt/I2CBaseNode.hpp"


#include <boost/lexical_cast.hpp>
#include <boost/unordered/unordered_map.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "pdt/exception.hpp"
#include "pdt/Logger.hpp"
#include "pdt/toolbox.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(I2CBaseNode);

// PRIVATE CONST definitions
const std::string I2CBaseNode::kPreHi = "ps_hi";
const std::string I2CBaseNode::kPreLo = "ps_lo";
const std::string I2CBaseNode::kCtrl = "ctrl";
const std::string I2CBaseNode::kTx = "data";
const std::string I2CBaseNode::kRx = "data";
const std::string I2CBaseNode::kCmd = "cmd_stat";
const std::string I2CBaseNode::kStatus = "cmd_stat";

const uint32_t I2CBaseNode::kStartCmd = 0x80; // 1 << 7
const uint32_t I2CBaseNode::kStopCmd = 0x40;  // 1 << 6
const uint32_t I2CBaseNode::kReadFromSlaveCmd = 0x20; // 1 << 5
const uint32_t I2CBaseNode::kWriteToSlaveCmd = 0x10; // 1 << 4
const uint32_t I2CBaseNode::kAckCmd = 0x08; // 1 << 3
const uint32_t I2CBaseNode::kInterruptAck = 0x01; // 1
                                                  // 
//-----------------------------------------------------------------------------
I2CBaseNode::I2CBaseNode(const uhal::Node& aNode) : uhal::Node(aNode) {
    constructor();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void I2CBaseNode::constructor() {
    // 16 bit clock prescale factor.
    // formula: m_clockPrescale = (input_frequency / 5 / desired_frequency) -1
    // for typical IPbus applications: input frequency = IPbus clock = 31.x MHz
    // target frequency 100 kHz to play it safe (first revision of i2c standard),
    // but e.g. the SI5326 clock chip on the MP7 can do up to 400 kHz
    mClockPrescale = 0x40;
    
    // Build the list of slaves
    // Loop over node parameters. Each parameter becomes a slave node.
    const boost::unordered_map<std::string, std::string>& lPars = this->getParameters();
    boost::unordered_map<std::string, std::string>::const_iterator lIt;
    for ( lIt = lPars.begin(); lIt != lPars.end(); ++lIt ) {
        uint32_t slaveAddr = (boost::lexical_cast< pdt::stoul<uint32_t> > (lIt->second) & 0x7f);
        mSlavesAddresses.insert(std::make_pair( lIt->first, slaveAddr  ) );
    }
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CBaseNode::~I2CBaseNode() {
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::vector<std::string>
I2CBaseNode::getSlaves() const {
    std::vector<std::string> lSlaves;

    boost::copy(mSlavesAddresses | boost::adaptors::map_keys, std::back_inserter(lSlaves));
    return lSlaves;
    
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
I2CBaseNode::getSlaveAddress(const std::string& name) const {
    boost::unordered_map<std::string, uint8_t>::const_iterator lIt = mSlavesAddresses.find(name);
    if ( lIt == mSlavesAddresses.end() ) {
        pdt::I2CSlaveNotFound lExc( std::string("Slave \"") + name + "\" not found" );
        PDT_LOG(kError) << lExc.what();
        throw lExc;
    }
    return lIt->second;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
I2CBaseNode::readI2C(uint8_t aSlaveAddress, uint32_t i2cAddress) const {
    // write one word containing the address
    std::vector<uint8_t> array(1, i2cAddress & 0x7f);
    this->writeBlockI2C(aSlaveAddress, array);
    // request the content at the specific address
    return this->readBlockI2C(aSlaveAddress, 1) [0];
}
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
void I2CBaseNode::writeI2C(uint8_t aSlaveAddress, uint32_t i2cAddress, uint32_t data) const {
    std::vector<uint8_t> block(2);
    block[0] = (i2cAddress & 0xff);
    block[1] = (data & 0xff);
    this->writeBlockI2C(aSlaveAddress, block);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CBaseNode::writeBlockI2C(uint8_t aSlaveAddress, const std::vector<uint8_t>& array) const {
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
    // Set slave address in bits 7:1, and set bit 0 to zero (i.e. "write mode")
    getNode(kTx).write((aSlaveAddress << 1) & 0xfe);
    getClient().dispatch();
    // Set start and write bit in command reg
    // getNode(kCmd).write(0x90);
    getNode(kCmd).write(kStartCmd + kWriteToSlaveCmd);
    // Run the commands and wait for transaction to finish
    getClient().dispatch();
    waitUntilFinished();

    for (unsigned ibyte = 0; ibyte < array.size(); ibyte++) {
        uint8_t stop_bit = 0x00;

        if (ibyte == array.size() - 1) {
            // stop_bit = 0x40;
            stop_bit = kStopCmd;
        }

        // Set array to be written in transmit reg
        getNode(kTx).write(array[ibyte]);
        getClient().dispatch();
        // Set write and stop bit in command reg
        // getNode(kCmd).write(0x10 + stop_bit);
        getNode(kCmd).write(kWriteToSlaveCmd + stop_bit);
        // Run the commands and wait for transaction to finish
        getClient().dispatch();

        if (stop_bit) {
            waitUntilFinished(true, true);
        } else {
            waitUntilFinished(true, false);
        }
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint8_t>
I2CBaseNode::readBlockI2C(uint8_t aSlaveAddress, uint32_t numBytes) const {
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
    // Set slave address in bits 7:1, and set bit 0 to one
    // (i.e. we're writing an address to the bus and then want to read)
    getNode(kTx).write((aSlaveAddress << 1) | 0x01);
    getClient().dispatch();
    // Set start and write bit in command reg
    // getNode(kCmd).write(0x90);
    getNode(kCmd).write(kStartCmd + kWriteToSlaveCmd);
    // Run the commands and wait for transaction to finish
    getClient().dispatch();
    waitUntilFinished();
    std::vector<uint8_t> array;

    for (unsigned ibyte = 0; ibyte < numBytes; ibyte++) {
        // Set read bit, acknowledge and stop bit in command reg
        uint8_t stop_bit = 0x00;
        uint8_t ack_bit = 0x00;

        if (ibyte == numBytes - 1) {
            // stop_bit = 0x40;
            stop_bit = kStopCmd;
            // stop_bit = 0x10;
            ack_bit = kAckCmd;
        }

        // getNode(kCmd).write(0x20 + ack_bit + stop_bit);
        getNode(kCmd).write(kReadFromSlaveCmd + ack_bit + stop_bit);
        getClient().dispatch();

        // Wait for transaction to finish.
        // Don't expect an ACK, do expect bus free at finish.
        if (stop_bit) {
            waitUntilFinished(false, true);
        } else {
            waitUntilFinished(false, false);
        }

        uhal::ValWord<uint32_t> result = getNode(kRx).read();
        getClient().dispatch();
        array.push_back(result);
    }

    return array;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void I2CBaseNode::reset() const {
    // Resets the I2C bus
    //
    // This function does the following:
    //        1) Disables the I2C core
    //        2) Sets the clock prescale registers
    //        3) Enables the I2C core
    //        4) Sets all writable bus-master registers to default values
    // disable the I2C core
    getNode(kCtrl).write(0x00);
    getClient().dispatch();
    // set the clock prescale
    getNode(kPreHi).write((mClockPrescale & 0xff00) >> 8);
    getClient().dispatch();
    getNode(kPreLo).write(mClockPrescale & 0xff);
    getClient().dispatch();
    // enable the I2C core
    getNode(kCtrl).write(0x80);
    getClient().dispatch();
    // set all writable bus-master registers to default values
    getNode(kTx).write(0x00);
    getClient().dispatch();
    getNode(kCmd).write(0x00);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void I2CBaseNode::waitUntilFinished(bool requireAcknowledgement,
        bool requireBusIdleAtEnd) const {
    // Ensures the current bus transaction has finished successfully
    // before allowing further I2C bus transactions
    // This method monitors the status register
    // and will not allow execution to continue until the
    // I2C bus has completed properly.  It will throw an exception
    // if it picks up bus problems or a bus timeout occurs.
    const unsigned maxRetry = 20;
    unsigned attempt = 1;
    bool receivedAcknowledge, busy;

    while (attempt <= maxRetry) {
        usleep(10);
        // Get the status
        uhal::ValWord<uint32_t> i2c_status = getNode(kStatus).read();
        getClient().dispatch();
        receivedAcknowledge = !(i2c_status & 0x80);
        busy = (i2c_status & 0x40);
        bool arbitrationLost = (i2c_status & 0x20);
        bool transferInProgress = (i2c_status & 0x02);
        //bool interruptFlag = (i2c_status & 0x01);

        if (arbitrationLost) {
            // This is an instant error at any time
            pdt::I2CException lExc("OpenCoresI2CMasterNode error: bus arbitration lost. Is another application running?");
            PDT_LOG(kError) << lExc.what();
            throw lExc;
        }

        if (!transferInProgress) {
            // The transfer looks to have completed successfully,
            // pending further checks
            break;
        }

        attempt += 1;
    }

    // At this point, we've either had too many retries, or the
    // Transfer in Progress (TIP) bit went low.  If the TIP bit
    // did go low, then we do a couple of other checks to see if
    // the bus operated as expected:

    if (attempt > maxRetry) {
        pdt::I2CException lExc("OpenCoresI2CMasterNode error: Transaction timeout - the 'Transfer in Progress' bit remained high for too long");
        PDT_LOG(kError) << lExc.what();
        throw lExc;
    }

    if (requireAcknowledgement && !receivedAcknowledge) {
        pdt::I2CException lExc("OpenCoresI2CMasterNode error: No acknowledge received");
        PDT_LOG(kError) << lExc.what();
        throw lExc;
    }

    if (requireBusIdleAtEnd && busy) {
        pdt::I2CException lExc("OpenCoresI2CMasterNode error: Transfer finished but bus still busy");
        PDT_LOG(kError) << lExc.what();
        throw lExc;
    }
}
//-----------------------------------------------------------------------------

} // namespace pdt

