/**
 * @file I2CSFPNode.hpp
 *
 * I2CSFPSlave and I2CSFPNode are classes providing an I2C interface
 * to SFPs.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_I2CSFPNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_I2CSFPNODE_HPP_

#include "pdt/I2CSlave.hpp"
#include "pdt/I2CMasterNode.hpp"
#include "pdt/toolbox.hpp"
#include "pdt/timingmon/Structs.hpp"
#include "TimingIssues.hpp"

#include "ers/ers.h"

#include <map>

namespace dunedaq {
namespace pdt {

/**
 * @class      I2CSFPSlave
 *
 * @brief      I2C slave class to control SFP transceivers.
 * @author     Alessandro Thea
 * @date       April 2018
 */
class I2CSFPSlave : public I2CSlave {
protected:
    const std::vector<uint32_t> m_calibration_parameter_start_addresses;

public:
    I2CSFPSlave(const I2CMasterNode* i2c_master, uint8_t i2c_device_address);
    virtual ~I2CSFPSlave();
    
    /**
     * @brief      Check if SFP responds
     *
     */
    void sfp_reachable() const;

    /**
     * @brief      Check if DDM is supported
     *
     */
    void ddm_available() const;

    /**
     * @brief      Read the raw SFP temperature
     *
     */
    std::pair<double, double>  read_calibration_parameter_pair(uint32_t calib_parameter_id) const;

    /**
     * @brief      Read the raw SFP temperature
     *
     */
    double read_temperature_raw() const;

    /**
     * @brief      Read the calibrated SFP temperature
     *
     */
    double read_temperature() const;

    /**
     * @brief      Read the raw SFP voltage
     *
     */
    double read_voltage_raw() const;

    /**
     * @brief      Read the calibrated SFP voltage
     *
     */
    double read_voltage() const;

    /**
     * @brief      Read the raw SFP temperature
     *
     */
    double read_rx_power_raw() const;

    /**
     * @brief      Read the raw SFP temperature
     *
     */
    double read_rx_ower() const;

    /**
     * @brief      Read the raw SFP tx power
     *
     */
    double read_tx_power_raw() const;

    /**
     * @brief      Read the calibrated SFP tx power
     *
     */
    double read_tx_power() const;

    /**
     * @brief      Read the raw SFP current
     *
     */
    double read_current_raw() const;

    /**
     * @brief      Read the calibrated SFP current
     *
     */
    double read_current() const;

    /**
     * @brief      Read the vendor name
     *
     */
    std::string read_vendor_name() const;

    /**
     * @brief      Read the vendor name
     *
     */
    std::string read_vendor_part_number() const;

    /**
     * @brief      Read the SFP serial number
     *
     */
    std::string read_serial_number() const;

    /**
     * @brief      Find out if SFP supports DDM
     *
     */
    bool read_ddm_support_bit() const;

    /**
     * @brief      Find out if SFP supports soft tx laser disable
     *
     */
    bool read_soft_tx_control_support_bit() const;

    /**
     * @brief      Read the value of the soft tx disable control bit
     *
     */
    bool read_soft_tx_control_state() const;

    /**
     * @brief      Read the state of the tx disable control pin
     *
     */
    bool read_tx_disable_pin_state() const;
    
    /**
     * @brief      Read whether the SFP has seperate I2C addresses, or if a special I2C address swap is required. True = address swap required
     *
     */
    bool read_i2c_reg_addressSwapBit() const;

    /**
     * @brief     Switch on or off the SFP tx laser via the soft control bit
     *
     */
    void switch_soft_tx_control_bit(bool turn_on) const;

    /**
     * @brief      Get SFP status
     */
    std::string get_status(bool print_out=false) const;

    /**
     * @brief      Get and fill SFP hardware data
     */
    void get_info(timingmon::TimingSFPMonitorData& mon_data) const;
};

/**
 * @class      I2CExpanderNode
 *
 * @brief      uhal::Node implementing single I2C Master Slave connection to
 *             control SFP expander chips.
 * @author     Alessandro Thea
 * @date       April 2018
 */
class I2CSFPNode : public I2CMasterNode, public I2CSFPSlave {
    UHAL_DERIVEDNODE(I2CSFPNode)
public:
    I2CSFPNode(const uhal::Node& node);
    I2CSFPNode(const I2CSFPNode& node);
    virtual ~I2CSFPNode();

};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SFPEXPANDERNODE_HPP_
