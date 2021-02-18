#include "pdt/SIChipSlave.hpp"

// PDT headers
#include "pdt/toolbox.hpp"
#include "ers/ers.h"

#include <boost/tuple/tuple.hpp>

#include <fstream>
#include <sstream>

namespace dunedaq {
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
SIChipSlave::read_page( ) const {

    ERS_DEBUG(2, "<- Reading page ");

    // Read from the page address (0x1?)
    return read_i2c(0x1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SIChipSlave::switch_page( uint8_t aPage ) const {

    // Prepare a data block with address and new page
    // std::vector<uint8_t> lData = {0x1, aPage};
    ERS_DEBUG(2, "-> Switching to page " 
        << format_reg_value((uint32_t)aPage));
    write_i2c(0x1, aPage);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
SIChipSlave::read_device_version( ) const {

    // Go to the right page
    switch_page(0x0);
    // Read 2 words from 0x2
    auto lVersion = read_i2cArray(0x2, 2);
    
    return (((uint32_t)lVersion[1] << 8) + (uint32_t)lVersion[0]);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
SIChipSlave::read_clock_register( uint16_t aAddr ) const {

    uint8_t lRegAddr = (aAddr & 0xff);
    uint8_t lPageAddr = (aAddr >> 8) & 0xff;
    std::stringstream debug_stream;
    debug_stream << std::showbase << std::hex 
        << "Read Address " << (uint32_t)aAddr 
        << " reg: " << (uint32_t)lRegAddr 
        << " page: " << (uint32_t)lPageAddr;
    ERS_DEBUG(2, debug_stream.str());
    // Change page only when required.
    // (The SI5344 don't like to have the page register id to be written all the time.)
    uint8_t lCurrentPage = read_page();
    if ( lPageAddr != lCurrentPage ) {
        switch_page(lPageAddr);
    }

    // Read the register
    return read_i2c( lRegAddr );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SIChipSlave::write_clock_register( uint16_t aAddr, uint8_t aData ) const {

    uint8_t lRegAddr = (aAddr & 0xff);
    uint8_t lPageAddr = (aAddr >> 8) & 0xff;

    std::stringstream debug_stream;
    debug_stream << std::showbase << std::hex 
        << "Write Address " << (uint32_t)aAddr 
        << " reg: " << (uint32_t)lRegAddr 
        << " page: " << (uint32_t)lPageAddr;
    ERS_DEBUG(2, debug_stream.str());
    // Change page only when required.
    // (The SI5344 don't like to have the page register id to be written all the time.)
    uint8_t lCurrentPage = read_page();
    if ( lPageAddr != lCurrentPage ) {
        switch_page(lPageAddr);
    }

    return write_i2c( lRegAddr, aData );
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq