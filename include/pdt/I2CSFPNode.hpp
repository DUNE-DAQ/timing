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
    const std::vector<uint32_t> mCalibrationParameterStartAddresses;

public:
    I2CSFPSlave(const I2CMasterNode* aMaster, uint8_t aSlaveAddress);
    virtual ~I2CSFPSlave();
    
    /**
     * @brief      Check if SFP responds
     *
     */
    void sfpReachable() const;

    /**
     * @brief      Check if DDM is supported
     *
     */
    void ddmAvailable() const;

    /**
     * @brief      Read the raw SFP temperature
     *
     */
    std::pair<double, double>  readCalibrationParameterPair(uint32_t aCalibID) const;

    /**
     * @brief      Read the raw SFP temperature
     *
     */
    double readTemperatureRaw() const;

    /**
     * @brief      Read the calibrated SFP temperature
     *
     */
    double readTemperature() const;

    /**
     * @brief      Read the raw SFP voltage
     *
     */
    double readVoltageRaw() const;

    /**
     * @brief      Read the calibrated SFP voltage
     *
     */
    double readVoltage() const;

    /**
     * @brief      Read the raw SFP temperature
     *
     */
    double readRxPowerRaw() const;

    /**
     * @brief      Read the raw SFP temperature
     *
     */
    double readRxPower() const;

    /**
     * @brief      Read the raw SFP tx power
     *
     */
    double readTxPowerRaw() const;

    /**
     * @brief      Read the calibrated SFP tx power
     *
     */
    double readTxPower() const;

    /**
     * @brief      Read the raw SFP current
     *
     */
    double readCurrentRaw() const;

    /**
     * @brief      Read the calibrated SFP current
     *
     */
    double readCurrent() const;

    /**
     * @brief      Read the vendor name
     *
     */
    std::string readVendorName() const;

    /**
     * @brief      Read the vendor name
     *
     */
    std::string readVendorPartNumber() const;

    /**
     * @brief      Read the SFP serial number
     *
     */
    std::string readSerialNumber() const;

    /**
     * @brief      Find out if SFP supports DDM
     *
     */
    bool readDDMSupportBit() const;

    /**
     * @brief      Find out if SFP supports soft tx laser disable
     *
     */
    bool readSoftTxControlSupportBit() const;

    /**
     * @brief      Read the value of the soft tx disable control bit
     *
     */
    bool readSoftTxControlState() const;

    /**
     * @brief      Read the state of the tx disable control pin
     *
     */
    bool readTxDisablePinState() const;
    
    /**
     * @brief      Read whether the SFP has seperate I2C addresses, or if a special I2C address swap is required. True = address swap required
     *
     */
    bool readI2CAddressSwapBit() const;

    /**
     * @brief     Switch on or off the SFP tx laser via the soft control bit
     *
     */
    void switchSoftTxControlBit(bool aOn) const;

    /**
     * @brief      Get SFP status
     */
    std::string get_status(bool aPrint=false) const;
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
    I2CSFPNode(const uhal::Node& aNode);
    I2CSFPNode(const I2CSFPNode& aOther);
    virtual ~I2CSFPNode();

};

} // namespace pdt
} // namespace dunedaq

#endif /* TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SFPEXPANDERNODE_HPP_ */
