#ifndef __PDT_SICHIPSLAVE_HPP__
#define __PDT_SICHIPSLAVE_HPP__

#include <map>

#include "pdt/I2CSlave.hpp"

namespace pdt 
{


/**
 * @class      SIChipSlave
 *
 * @brief      Utility class to interface to SI chips.
 * @author     Alessandro Thea
 * @date       May 2018
 */
class SIChipSlave : public I2CSlave {
public:
    SIChipSlave(const I2CBaseNode* aMaster, uint8_t aSlaveAddress);
    virtual ~SIChipSlave();

    /**
     * @brief      Reads the current page.
     *
     * @return     { description_of_the_return_value }
     */
    uint8_t readPage() const;
    
    void switchPage(uint8_t aPage) const;

    uint32_t readDeviceVersion() const;

    uint8_t readClockRegister( uint16_t aAddr ) const;

    void writeClockRegister( uint16_t aAddr, uint8_t aData) const;


};

} // namespace pdt

#endif /* __PDT_SICHIPSLAVE_HPP__ */