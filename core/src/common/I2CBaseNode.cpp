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

const uint8_t I2CBaseNode::kStartCmd = 0x80; // 1 << 7
const uint8_t I2CBaseNode::kStopCmd = 0x40;  // 1 << 6
const uint8_t I2CBaseNode::kReadFromSlaveCmd = 0x20; // 1 << 5
const uint8_t I2CBaseNode::kWriteToSlaveCmd = 0x10; // 1 << 4
const uint8_t I2CBaseNode::kAckCmd = 0x08; // 1 << 3
const uint8_t I2CBaseNode::kInterruptAck = 0x01; // 1

const uint8_t I2CBaseNode::kReceivedAckBit = 0x80;// recvdack = 0x1 << 7
const uint8_t I2CBaseNode::kBusyBit = 0x40;// busy = 0x1 << 6
const uint8_t I2CBaseNode::kArbitrationLostBit = 0x20;// arblost = 0x1 << 5
const uint8_t I2CBaseNode::kInProgressBit = 0x2;// inprogress = 0x1 << 1
const uint8_t I2CBaseNode::kInterruptBit = 0x1;// interrupt = 0x1

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
    // mClockPrescale = 0x40;
    mClockPrescale = 0x100;
    
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
    // // write one word containing the address
    // std::vector<uint8_t> array(1, i2cAddress & 0x7f);
    // this->writeBlockI2C(aSlaveAddress, array);
    // // request the content at the specific address
    // return this->readBlockI2C(aSlaveAddress, 1) [0];
    return this->readI2CArray(aSlaveAddress, i2cAddress, 1)[0];
}
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
void
I2CBaseNode::writeI2C(uint8_t aSlaveAddress, uint32_t i2cAddress, uint8_t aData, bool aSendStop) const {
    // std::vector<uint8_t> block(2);
    // block[0] = (i2cAddress & 0xff);
    // block[1] = (aData & 0xff);
    // this->writeBlockI2C(aSlaveAddress, block);

    this->writeI2CArray( aSlaveAddress, i2cAddress, {aData}, aSendStop);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint8_t>
I2CBaseNode::readI2CArray(uint8_t aSlaveAddress, uint32_t i2cAddress, uint32_t aNumWords) const {
    // std::cout << "Reading " << aNumWords << " from " << std::showbase <<  std::hex << i2cAddress << " on " << (uint32_t)aSlaveAddress << std::endl; // HACK
    // write one word containing the address
    std::vector<uint8_t> lArray(1, i2cAddress & 0xff);
    this->writeBlockI2C(aSlaveAddress, lArray);
    // request the content at the specific address
    return this->readBlockI2C(aSlaveAddress, aNumWords);
}
//-----------------------------------------------------------------------------
 

//-----------------------------------------------------------------------------
void
I2CBaseNode::writeI2CArray(uint8_t aSlaveAddress, uint32_t i2cAddress, std::vector<uint8_t> aData, bool aSendStop) const {
    // std::cout << "Writing " << aData.size() << " from " << std::showbase <<  std::hex << i2cAddress << " on " << (uint32_t)aSlaveAddress << std::endl; // HACK
    std::vector<uint8_t> block(aData.size() + 1);
    block[0] = (i2cAddress & 0xff);

    for ( size_t i(0); i < aData.size(); ++i )
        block[i+1] = aData[i];

    this->writeBlockI2C(aSlaveAddress, block, aSendStop);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CBaseNode::writeBlockI2C(uint8_t aSlaveAddress, const std::vector<uint8_t>& aArray, bool aSendStop) const {
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
    
    // std::cout << "+ write " << (uint32_t)((aSlaveAddress << 1) & 0xfe) << std::endl; // HACK

    // TODO: Check if this is still required
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

    for (unsigned iByte = 0; iByte < aArray.size(); iByte++) {
        // uint8_t lStopBit = 0x00;

        // if (iByte == aArray.size() - 1 && aSendStop) {
        //     // lStopBit = 0x40;
        //     lStopBit = kStopCmd;
        // }
        std::cout << "wi2c >> i=" << iByte << " d=" << (uint32_t)aArray[iByte]  << " cmd=" << (uint32_t)kWriteToSlaveCmd << std::endl; // HACK

        // Set aArray to be written in transmit reg
        getNode(kTx).write(aArray[iByte]);
        getClient().dispatch();
        // Set write and stop bit in command reg
        // getNode(kCmd).write(0x10 + lStopBit);
        // getNode(kCmd).write(kWriteToSlaveCmd + lStopBit);
        getNode(kCmd).write(kWriteToSlaveCmd);
        // Run the commands and wait for transaction to finish
        getClient().dispatch();

        // if (lStopBit) {
        //     waitUntilFinished(true, true);
        // } else {
            waitUntilFinished(true, false);
        // }

    }

    if (aSendStop) {
        getNode(kCmd).write(kStopCmd);
        getClient().dispatch();
        waitUntilFinished(true, true);
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<uint8_t>
I2CBaseNode::readBlockI2C(uint8_t aSlaveAddress, uint32_t lNumBytes) const {
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
    
    // std::cout << "+ read" << std::endl; // HACK

    // TODO: Check if this is still required
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
    // std::cout << ">> " << ((aSlaveAddress << 1) | 0x01) << " cmd " << kStartCmd + kWriteToSlaveCmd << std::endl;// HACK
    waitUntilFinished();
    std::vector<uint8_t> lArray;

    for (unsigned ibyte = 0; ibyte < lNumBytes; ibyte++) {
        // Set read bit, acknowledge and stop bit in command reg
        // uint8_t lStopBit = 0x00;
        // uint8_t lAckBit = 0x00;
        
        uint8_t lCmd;
        lCmd = kReadFromSlaveCmd;

        if (ibyte == lNumBytes - 1) {
            // lStopBit = 0x40;
            // lStopBit = kStopCmd;
            // lAckBit = 0x10;
            // lAckBit = kAckCmd;
            lCmd += (kStopCmd | kAckCmd);
        }

        // getNode(kCmd).write(0x20 + lAckBit + lStopBit);
        // getNode(kCmd).write(kReadFromSlaveCmd + lAckBit + lStopBit);
        getNode(kCmd).write(lCmd);
        getClient().dispatch();
        // std::cout << ">> cmd " << kReadFromSlaveCmd + lAckBit + lStopBit << std::endl;// HACK


        // Wait for transaction to finish.
        // Don't expect an ACK, do expect bus free at finish.
        // if (lStopBit) {
            // waitUntilFinished(false, true);
        // } else {
            waitUntilFinished(false, false);
        // }

        uhal::ValWord<uint32_t> lResult = getNode(kRx).read();
        getClient().dispatch();
        lArray.push_back(lResult);
    }

    return lArray;
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
    // getClient().dispatch();
    getNode(kPreLo).write(mClockPrescale & 0xff);
    getClient().dispatch();
    // enable the I2C core
    getNode(kCtrl).write(0x80);
    getClient().dispatch();
    // set all writable bus-master registers to default values
    getNode(kTx).write(0x00);
    // getClient().dispatch();
    getNode(kCmd).write(0x00);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void I2CBaseNode::waitUntilFinished(bool requireAcknowledgement, bool requireBusIdleAtEnd) const {
    // Ensures the current bus transaction has finished successfully
    // before allowing further I2C bus transactions
    // This method monitors the status register
    // and will not allow execution to continue until the
    // I2C bus has completed properly.  It will throw an exception
    // if it picks up bus problems or a bus timeout occurs.
    const unsigned lMaxRetry = 20;
    unsigned lAttempt = 1;
    bool lReceivedAcknowledge, lBusy;

    while (lAttempt <= lMaxRetry) {
        usleep(10);
        // Get the status
        uhal::ValWord<uint32_t> i2c_status = getNode(kStatus).read();
        getClient().dispatch();
        // lReceivedAcknowledge = !(i2c_status & 0x80);
        lReceivedAcknowledge = !(i2c_status & kReceivedAckBit);
        // lBusy = (i2c_status & 0x40);
        lBusy = (i2c_status & kBusyBit);
        // bool arbitrationLost = (i2c_status & 0x20);
        bool arbitrationLost = (i2c_status & kArbitrationLostBit);
        // bool transferInProgress = (i2c_status & 0x02);
        bool transferInProgress = (i2c_status & kInProgressBit);
        //bool interruptFlag = (i2c_status & 0x01);
        //
        // std::cout << i2c_status << "   " << std::dec << lAttempt << std::hex << std::endl; // HACK

        if (arbitrationLost) {
            // This is an instant error at any time
            pdt::I2CException lExc("I2C error: bus arbitration lost. Is another application running?");
            PDT_LOG(kError) << lExc.what();
            throw lExc;
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
        pdt::I2CException lExc("I2C error: Transaction timeout - the 'Transfer in Progress' bit remained high for too long");
        PDT_LOG(kError) << lExc.what();
        throw lExc;
    }

    if (requireAcknowledgement && !lReceivedAcknowledge) {
        pdt::I2CException lExc("I2C error: No acknowledge received");
        PDT_LOG(kError) << lExc.what();
        throw lExc;
    }

    if (requireBusIdleAtEnd && lBusy) {
        pdt::I2CException lExc("I2C error: Transfer finished but bus still busy");
        PDT_LOG(kError) << lExc.what();
        throw lExc;
    }
}
//-----------------------------------------------------------------------------

} // namespace pdt

