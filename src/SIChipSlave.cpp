#include "timing/SIChipSlave.hpp"

// PDT headers
#include "timing/toolbox.hpp"
#include "ers/ers.hpp"

#include <boost/tuple/tuple.hpp>

#include <fstream>
#include <sstream>

namespace dunedaq {
namespace timing {

//-----------------------------------------------------------------------------
SIChipSlave::SIChipSlave( const I2CMasterNode* i2c_master, uint8_t address ) :
I2CSlave( i2c_master, address ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SIChipSlave::~SIChipSlave( ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
SIChipSlave::read_page( ) const {

    TLOG_DEBUG(2) << "<- Reading page ";

    // Read from the page address (0x1?)
    return read_i2c(0x1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SIChipSlave::switch_page( uint8_t page ) const {

    // Prepare a data block with address and new page
    // std::vector<uint8_t> lData = {0x1, page};
    TLOG_DEBUG(2) << "-> Switching to page " 
        << format_reg_value((uint32_t)page);
    write_i2c(0x1, page);
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
SIChipSlave::read_clock_register( uint16_t address ) const {

    uint8_t lRegAddr = (address & 0xff);
    uint8_t lPageAddr = (address >> 8) & 0xff;
    std::stringstream debug_stream;
    debug_stream << std::showbase << std::hex 
        << "Read Address " << (uint32_t)address 
        << " reg: " << (uint32_t)lRegAddr 
        << " page: " << (uint32_t)lPageAddr;
    TLOG_DEBUG(2) << debug_stream.str();
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
SIChipSlave::write_clock_register( uint16_t address, uint8_t data ) const {

    uint8_t lRegAddr = (address & 0xff);
    uint8_t lPageAddr = (address >> 8) & 0xff;

    std::stringstream debug_stream;
    debug_stream << std::showbase << std::hex 
        << "Write Address " << (uint32_t)address 
        << " reg: " << (uint32_t)lRegAddr 
        << " page: " << (uint32_t)lPageAddr;
    TLOG_DEBUG(2) << debug_stream.str();
    // Change page only when required.
    // (The SI5344 don't like to have the page register id to be written all the time.)
    uint8_t lCurrentPage = read_page();
    if ( lPageAddr != lCurrentPage ) {
        switch_page(lPageAddr);
    }

    return write_i2c( lRegAddr, data );
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq