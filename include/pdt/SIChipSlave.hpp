/**
 * @file SIChipSlave.hpp
 *
 * SIChipSlave is a class providing an interface
 * to SI I2C devices.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SICHIPSLAVE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SICHIPSLAVE_HPP_

#include "pdt/I2CSlave.hpp"

#include "ers/Issue.hpp"

#include <map>

namespace dunedaq {
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
    SIChipSlave(const I2CMasterNode* i2c_master, uint8_t i2c_device_address);
    virtual ~SIChipSlave();

    /**
     * @brief      Reads the current page.
     *
     * @return     { description_of_the_return_value }
     */
    uint8_t read_page() const;
    
    /**
     * @brief      { function_description }
     *
     * @param[in]  page  A page
     */
    void switch_page(uint8_t page) const;

    /**
     * @brief      Reads a device version.
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t read_device_version() const;

    /**
     * @brief      Reads a clock register.
     *
     * @param[in]  address  A address
     *
     * @return     { description_of_the_return_value }
     */
    uint8_t read_clock_register( uint16_t address ) const;

    /**
     * @brief      Writes a clock register.
     *
     * @param[in]  address  A address
     * @param[in]  data  A data
     */
    void write_clock_register( uint16_t address, uint8_t data) const;


};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SICHIPSLAVE_HPP_ 