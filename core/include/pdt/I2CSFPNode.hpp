#ifndef __PDT_I2CSFPNODE_HPP__
#define __PDT_I2CSFPNODE_HPP__

#include <map>

#include "pdt/I2CSlave.hpp"
#include "pdt/I2CMasterNode.hpp"
#include "pdt/toolbox.hpp"
#include "pdt/exception.hpp"
namespace pdt
{
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
     * @brief     Switch on or off the SFP tx laser via the soft control bit. aOn=1: laster transmitting, soft tx disable bit = 0; aOn=0: laster NOT transmitting, soft tx disable bit = 1. 
     *
     */
    void switchSoftTxControl(bool aOn) const;

    /**
     * @brief      Get SFP status
     */
    std::string getStatus(bool aPrint=false) const;
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
    UHAL_DERIVEDNODE(I2CSFPNode);
public:
    I2CSFPNode(const uhal::Node& aNode);
    I2CSFPNode(const I2CSFPNode& aOther);
    virtual ~I2CSFPNode();

};

} // namespace pdt

#endif /* __PDT_SFPEXPANDERNODE_HPP__ */
