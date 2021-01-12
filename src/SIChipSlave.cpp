#include "pdt/SIChipSlave.hpp"

// PDT headers
#include "pdt/toolbox.hpp"
#include "pdt/Logger.hpp"

#include <boost/tuple/tuple.hpp>

#include <fstream>
#include <sstream>

namespace pdt {

//-----------------------------------------------------------------------------
SIChipSlave::SIChipSlave( const I2CMasterNode* aMaster, uint8_t aAddr ) :
I2CSlave( aMaster, aAddr ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SIChipSlave::~SIChipSlave( ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
SIChipSlave::readPage( ) const {

    PDT_LOG(kDebug) << "<- Reading page ";

    // Read from the page address (0x1?)
    return readI2C(0x1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SIChipSlave::switchPage( uint8_t aPage ) const {

    // Prepare a data block with address and new page
    // std::vector<uint8_t> lData = {0x1, aPage};
    PDT_LOG(kDebug) << "-> Switching to page " 
        << std::showbase
        << std:: hex
        << (uint32_t) aPage;
    writeI2C(0x1, aPage);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
SIChipSlave::readDeviceVersion( ) const {

    // Go to the right page
    switchPage(0x0);
    // Read 2 words from 0x2
    auto lVersion = readI2CArray(0x2, 2);
    
    return (((uint32_t)lVersion[1] << 8) + (uint32_t)lVersion[0]);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
SIChipSlave::readClockRegister( uint16_t aAddr ) const {

    uint8_t lRegAddr = (aAddr & 0xff);
    uint8_t lPageAddr = (aAddr >> 8) & 0xff;
    PDT_LOG(kDebug) << std::showbase << std::hex 
        << "Read Address " << (uint32_t)aAddr 
        << " reg: " << (uint32_t)lRegAddr 
        << " page: " << (uint32_t)lPageAddr;
    // Change page only when required.
    // (The SI5344 don't like to have the page register id to be written all the time.)
    uint8_t lCurrentPage = readPage();
    if ( lPageAddr != lCurrentPage ) {
        switchPage(lPageAddr);
    }

    // Read the register
    return readI2C( lRegAddr );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SIChipSlave::writeClockRegister( uint16_t aAddr, uint8_t aData ) const {

    uint8_t lRegAddr = (aAddr & 0xff);
    uint8_t lPageAddr = (aAddr >> 8) & 0xff;

    PDT_LOG(kDebug) << std::showbase << std::hex 
        << "Write Address " << (uint32_t)aAddr 
        << " reg: " << (uint32_t)lRegAddr 
        << " page: " << (uint32_t)lPageAddr;
    // Change page only when required.
    // (The SI5344 don't like to have the page register id to be written all the time.)
    uint8_t lCurrentPage = readPage();
    if ( lPageAddr != lCurrentPage ) {
        switchPage(lPageAddr);
    }

    return writeI2C( lRegAddr, aData );
}
//-----------------------------------------------------------------------------


} // namespace pdt