#include "pdt/I2CSFPNode.hpp"

namespace dunedaq {
namespace pdt {

//-----------------------------------------------------------------------------
I2CSFPSlave::I2CSFPSlave( const I2CMasterNode* aMaster, uint8_t aAddr ) :
I2CSlave( aMaster, aAddr ),
mCalibrationParameterStartAddresses({0x4C, 0x50, 0x54, 0x58}) // laser current, tx_pwr, temp, voltage
{
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CSFPSlave::~I2CSFPSlave( ) {
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void 
I2CSFPSlave::sfpReachable() const {
    if (!this->ping()) {
        throw SFPUnreachable(ERS_HERE, "I2CSFPSlave", getMasterId());
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void 
I2CSFPSlave::ddmAvailable() const {
    if (!readDDMSupportBit()) {
        throw SFPDDMUnsupported(ERS_HERE, "I2CSFPSlave", getMasterId());
    } else {
        if (readI2CAddressSwapBit()) {
            throw SFPDDMI2CAddressSwapUnsupported(ERS_HERE, "I2CSFPSlave", getMasterId());
        }
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::pair<double, double> 
I2CSFPSlave::readCalibrationParameterPair(uint32_t aCalibID) const {
    
    ddmAvailable();

    auto lParameterArray = this->readI2CArray(0x51, mCalibrationParameterStartAddresses.at(aCalibID), 0x4);

    // slope
    double lSlope = lParameterArray.at(0) + (lParameterArray.at(1) / 256.0);

    uint32_t lOffsetRaw = (lParameterArray.at(2) & 0x7f) << 8 | lParameterArray.at(3);

    // eighth bit corresponds to sign
    double lOffset = lParameterArray.at(2) & (1UL << 7) ? lOffsetRaw - 0x8000 : lOffsetRaw;

    return std::make_pair(lSlope, lOffset);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::readTemperatureRaw() const {
    ddmAvailable();
    auto lTempArray = this->readI2CArray(0x51, 0x60, 0x2);
    
    // bit 7 corresponds to temperature sign, 0 for pos, 1 for neg
    double lTemp = lTempArray.at(0) & (1UL << 7)  ? (lTempArray.at(0) & 0x7f) - 0xff : lTempArray.at(0);
    return lTemp + (lTempArray.at(1) / 256.0);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::readTemperature() const {
    auto lTempRaw = readTemperatureRaw();
    auto lTempCalibPair = readCalibrationParameterPair(0x2);
    return lTempRaw*lTempCalibPair.first + lTempCalibPair.second;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::readVoltageRaw() const {
    ddmAvailable();
    auto lVoltageArray = this->readI2CArray(0x51, 0x62, 0x2);
    return (lVoltageArray.at(0) << 8) | lVoltageArray.at(1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::readVoltage() const {
    auto lVoltageRaw = this->readVoltageRaw();
    auto lTempCalibPair = readCalibrationParameterPair(0x3);
    return ((lVoltageRaw*lTempCalibPair.first) + lTempCalibPair.second)*1e-4;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::readRxPowerRaw() const {
    ddmAvailable();
    auto lRxPowerArray = this->readI2CArray(0x51, 0x68, 0x2);
    return (lRxPowerArray.at(0) << 8) | lRxPowerArray.at(1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::readRxPower() const {
    auto lRxPowerRaw = this->readRxPowerRaw();
    // rx power calib constants, 5 4-byte parameters, IEEE 754 float encoding
    std::vector<uint32_t> lRxParamStartAdr = {0x48, 0x44, 0x40, 0x3C, 0x38};
    std::vector<double> lRxParameters;
    for (auto it = lRxParamStartAdr.begin(); it != lRxParamStartAdr.end(); ++it) {
        uint32_t lParameterBits = 0;
        auto lParameterArray = this->readI2CArray(0x51, *it, 0x4);
        
        for (auto jt = lParameterArray.begin(); jt != lParameterArray.end(); ++jt) lParameterBits = (lParameterBits << 8) | *jt;

        // convert the 32 bits to a float according IEEE 754
        double lParameter = convertBitsToFloat(lParameterBits);
        lRxParameters.push_back(lParameter);
    }

    double lRxPowerCalib = 0;
    for (uint32_t i=0; i < lRxParameters.size(); ++i) {
        double lParameter = lRxParameters.at(i);
        lRxPowerCalib = lRxPowerCalib + (lParameter*pow(lRxPowerRaw,i));
    }
    return lRxPowerCalib*0.1;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::readTxPowerRaw() const {
    ddmAvailable();
    auto lTxPowerArray = this->readI2CArray(0x51, 0x66, 0x2);
    return (lTxPowerArray.at(0) << 8) | lTxPowerArray.at(1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::readTxPower() const {
    auto lTxPowerRaw = this->readTxPowerRaw();
    auto lTempCalibPair = readCalibrationParameterPair(0x1);
    return ((lTxPowerRaw*lTempCalibPair.first) + lTempCalibPair.second)*0.1;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::readCurrentRaw() const {
    ddmAvailable();
    auto lCurrentArray = this->readI2CArray(0x51, 0x64, 0x2);
    return (lCurrentArray.at(0) << 8) | lCurrentArray.at(1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::readCurrent() const {
    auto lCurrentRaw = this->readCurrentRaw();
    auto lTempCalibPair = readCalibrationParameterPair(0x0);
    return ((lCurrentRaw*lTempCalibPair.first) + lTempCalibPair.second)*0.002;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
I2CSFPSlave::readVendorName() const {
    sfpReachable();

    std::stringstream lVendorName;
    auto lVendorNameCharacters = this->readI2CArray(0x14, 0x10);
    for (auto it = lVendorNameCharacters.begin(); it != lVendorNameCharacters.end(); ++it) {
        lVendorName << *it;
    }
    return lVendorName.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
I2CSFPSlave::readVendorPartNumber() const {
    sfpReachable();

    std::stringstream lVendorPN;
    auto lVendorPNCharacters = this->readI2CArray(0x28, 0x10);
    for (auto it = lVendorPNCharacters.begin(); it != lVendorPNCharacters.end(); ++it) {
        lVendorPN << *it;
    }
    return lVendorPN.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
I2CSFPSlave::readSerialNumber() const {
    sfpReachable();

    std::stringstream lSerialNumber;
    auto lSerialNumberCharacters = this->readI2CArray(0x44, 0x10);
    for (auto it = lSerialNumberCharacters.begin(); it != lSerialNumberCharacters.end(); ++it) {
        lSerialNumber << *it;
    }
    return lSerialNumber.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CSFPSlave::readDDMSupportBit() const {
    sfpReachable();

    // Bit 6 of reg 5C tells us whether the SFP supports digital diagnostic monitoring (DDM)
    auto lDDMInfoByte = this->readI2C(0x5C);
    return lDDMInfoByte & 0x40;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CSFPSlave::readSoftTxControlSupportBit() const {
    sfpReachable();

    // Bit 6 of reg 5d tells us whether the soft tx control is implemented in this sfp
    auto lEnhancedOptionsByte = this->readI2C(0x5d);
    return lEnhancedOptionsByte & 0x40;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CSFPSlave::readSoftTxControlState() const {
    ddmAvailable();
    auto lOptStatusCtrlByte = this->readI2C(0x51, 0x6e);
    // Bit 6 tells us the state of the soft tx_disble register
    return lOptStatusCtrlByte & 0x40;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CSFPSlave::readTxDisablePinState() const {
    ddmAvailable();
    auto lOptStatusCtrlByte = this->readI2C(0x51, 0x6e);
    // Bit 7 tells us the state of tx_disble pin
    return lOptStatusCtrlByte & 0x80;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CSFPSlave::readI2CAddressSwapBit() const {
    sfpReachable();

    auto lDDMInfoByte = this->readI2C(0x5C);
    // Bit 2 of byte 5C tells us whether special I2C address change operations are needed to access the DDM area
    return lDDMInfoByte & 0x4;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CSFPSlave::switchSoftTxControlBit(bool aOn) const {
    ddmAvailable();

    if (!readSoftTxControlSupportBit()) {
        throw SoftTxLaserControlUnsupported(ERS_HERE, "I2CSFPSlave", getMasterId());
    }
    
    // Get optional status/control bits
    auto lOptStatusCtrlByte = this->readI2C(0x51, 0x6e);

    uint8_t lNewOptStatusCtrlByte;
    // Bit 6 of byte 0x6e controls the soft tx_disable
    if (aOn) {
        lNewOptStatusCtrlByte = lOptStatusCtrlByte & ~(1UL << 6);     
    } else {
        lNewOptStatusCtrlByte = lOptStatusCtrlByte | (1UL << 6);
    }
    this->writeI2C(0x51,0x6e,lNewOptStatusCtrlByte);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
I2CSFPSlave::get_status(bool aPrint) const {
    sfpReachable();
    
    std::stringstream lStatus;
    std::vector<std::pair<std::string, std::string>> lSFPInfo;

    // Vendor name
    lSFPInfo.push_back(std::make_pair("Vendor", readVendorName()));
     
    // Vendor part number
    lSFPInfo.push_back(std::make_pair("Part number", readVendorPartNumber()));

    // Serial number
    lSFPInfo.push_back(std::make_pair("Serial number", readSerialNumber()));
    
    // Does the SFP support DDM
    if (!readDDMSupportBit()) {
        ERS_LOG("DDM not available for SFP on I2C bus: " << getMasterId());
        lStatus << formatRegTable(lSFPInfo, "SFP status", {"", ""});
        if (aPrint) std::cout << lStatus.str();
        return lStatus.str();
    } else {
        if (readI2CAddressSwapBit()) {
            ERS_LOG("SFP DDM I2C address swap not supported. SFP on I2C bus: " << getMasterId());
            lStatus << formatRegTable(lSFPInfo, "SFP status", {"", ""});
            if (aPrint) std::cout << lStatus.str();
            return lStatus.str();
        }
    }

    std::stringstream lTempStream;
    lTempStream << std::dec << std::fixed << std::setprecision(2) << readTemperature() << " C";
    lSFPInfo.push_back(std::make_pair("Temperature", lTempStream.str()));
    
    std::stringstream lVoltageStream;
    lVoltageStream << std::dec << std::fixed << std::setprecision(2) << readVoltage() << " V";
    lSFPInfo.push_back(std::make_pair("Supply voltage", lVoltageStream.str()));

    std::stringstream lRxPowerStream;
    lRxPowerStream << std::dec << std::fixed << std::setprecision(2) << readRxPower() << " uW";
    lSFPInfo.push_back(std::make_pair("Rx power", lRxPowerStream.str()));

    std::stringstream lTxPowerStream;
    lTxPowerStream << std::dec << std::fixed << std::setprecision(2) << readTxPower() << " uW";
    lSFPInfo.push_back(std::make_pair("Tx power", lTxPowerStream.str()));

    std::stringstream lCurrentStream;
    lCurrentStream << std::dec << std::fixed << std::setprecision(2) << readCurrent() << " uA";
    lSFPInfo.push_back(std::make_pair("Tx current", lCurrentStream.str()));

    if (readSoftTxControlSupportBit()) {
        //lSFPInfo.push_back(std::make_pair("Soft Tx disbale supported",  "True"));
        lSFPInfo.push_back(std::make_pair("Tx disable bit" , std::to_string(readSoftTxControlState())));
    } else {
       lSFPInfo.push_back(std::make_pair("Soft Tx disbale supported",  "False"));
    }

    lSFPInfo.push_back(std::make_pair("Tx disable pin", std::to_string(readTxDisablePinState())));

    lStatus << formatRegTable(lSFPInfo, "SFP status", {"", ""});
    if (aPrint) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(I2CSFPNode)

//-----------------------------------------------------------------------------
I2CSFPNode::I2CSFPNode( const uhal::Node& aNode ) : I2CMasterNode(aNode), I2CSFPSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CSFPNode::I2CSFPNode( const I2CSFPNode& aOther ) : I2CMasterNode(aOther), I2CSFPSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CSFPNode::~I2CSFPNode() {
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq