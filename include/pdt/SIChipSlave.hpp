#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SICHIPSLAVE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SICHIPSLAVE_HPP_

#include <map>

#include "pdt/I2CSlave.hpp"

#include "ers/ers.h"

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
    SIChipSlave(const I2CMasterNode* aMaster, uint8_t aSlaveAddress);
    virtual ~SIChipSlave();

    /**
     * @brief      Reads the current page.
     *
     * @return     { description_of_the_return_value }
     */
    uint8_t readPage() const;
    
    /**
     * @brief      { function_description }
     *
     * @param[in]  aPage  A page
     */
    void switchPage(uint8_t aPage) const;

    /**
     * @brief      Reads a device version.
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t readDeviceVersion() const;

    /**
     * @brief      Reads a clock register.
     *
     * @param[in]  aAddr  A address
     *
     * @return     { description_of_the_return_value }
     */
    uint8_t readClockRegister( uint16_t aAddr ) const;

    /**
     * @brief      Writes a clock register.
     *
     * @param[in]  aAddr  A address
     * @param[in]  aData  A data
     */
    void writeClockRegister( uint16_t aAddr, uint8_t aData) const;


};

} // namespace pdt

#endif /* TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SICHIPSLAVE_HPP_ */