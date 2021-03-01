#include "pdt/I2CSFPNode.hpp"

namespace dunedaq {
namespace pdt {

//-----------------------------------------------------------------------------
I2CSFPSlave::I2CSFPSlave( const I2CMasterNode* i2c_master, uint8_t address ) :
I2CSlave( i2c_master, address ),
m_calibration_parameter_start_addresses({0x4C, 0x50, 0x54, 0x58}) // laser current, tx_pwr, temp, voltage
{
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CSFPSlave::~I2CSFPSlave( ) {
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void 
I2CSFPSlave::sfp_reachable() const {
    if (!this->ping()) {
        throw SFPUnreachable(ERS_HERE, "I2CSFPSlave", get_master_id());
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void 
I2CSFPSlave::ddm_available() const {
    if (!read_ddm_support_bit()) {
        throw SFPDDMUnsupported(ERS_HERE, "I2CSFPSlave", get_master_id());
    } else {
        if (read_i2c_reg_addressSwapBit()) {
            throw SFPDDMI2CAddressSwapUnsupported(ERS_HERE, "I2CSFPSlave", get_master_id());
        }
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::pair<double, double> 
I2CSFPSlave::read_calibration_parameter_pair(uint32_t calib_parameter_id) const {
    
    ddm_available();

    auto lParameterArray = this->read_i2cArray(0x51, m_calibration_parameter_start_addresses.at(calib_parameter_id), 0x4);

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
I2CSFPSlave::read_temperature_raw() const {
    ddm_available();
    auto lTempArray = this->read_i2cArray(0x51, 0x60, 0x2);
    
    // bit 7 corresponds to temperature sign, 0 for pos, 1 for neg
    double lTemp = lTempArray.at(0) & (1UL << 7)  ? (lTempArray.at(0) & 0x7f) - 0xff : lTempArray.at(0);
    return lTemp + (lTempArray.at(1) / 256.0);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::read_temperature() const {
    auto lTempRaw = read_temperature_raw();
    auto lTempCalibPair = read_calibration_parameter_pair(0x2);
    return lTempRaw*lTempCalibPair.first + lTempCalibPair.second;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::read_voltage_raw() const {
    ddm_available();
    auto lVoltageArray = this->read_i2cArray(0x51, 0x62, 0x2);
    return (lVoltageArray.at(0) << 8) | lVoltageArray.at(1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::read_voltage() const {
    auto lVoltageRaw = this->read_voltage_raw();
    auto lTempCalibPair = read_calibration_parameter_pair(0x3);
    return ((lVoltageRaw*lTempCalibPair.first) + lTempCalibPair.second)*1e-4;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::read_rx_power_raw() const {
    ddm_available();
    auto lRxPowerArray = this->read_i2cArray(0x51, 0x68, 0x2);
    return (lRxPowerArray.at(0) << 8) | lRxPowerArray.at(1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::read_rx_ower() const {
    auto lRxPowerRaw = this->read_rx_power_raw();
    // rx power calib constants, 5 4-byte parameters, IEEE 754 float encoding
    std::vector<uint32_t> lRxParamStartAdr = {0x48, 0x44, 0x40, 0x3C, 0x38};
    std::vector<double> lRxParameters;
    for (auto it = lRxParamStartAdr.begin(); it != lRxParamStartAdr.end(); ++it) {
        uint32_t lParameterBits = 0;
        auto lParameterArray = this->read_i2cArray(0x51, *it, 0x4);
        
        for (auto jt = lParameterArray.begin(); jt != lParameterArray.end(); ++jt) lParameterBits = (lParameterBits << 8) | *jt;

        // convert the 32 bits to a float according IEEE 754
        double lParameter = convert_bits_to_float(lParameterBits);
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
I2CSFPSlave::read_tx_power_raw() const {
    ddm_available();
    auto lTxPowerArray = this->read_i2cArray(0x51, 0x66, 0x2);
    return (lTxPowerArray.at(0) << 8) | lTxPowerArray.at(1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::read_tx_power() const {
    auto lTxPowerRaw = this->read_tx_power_raw();
    auto lTempCalibPair = read_calibration_parameter_pair(0x1);
    return ((lTxPowerRaw*lTempCalibPair.first) + lTempCalibPair.second)*0.1;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::read_current_raw() const {
    ddm_available();
    auto lCurrentArray = this->read_i2cArray(0x51, 0x64, 0x2);
    return (lCurrentArray.at(0) << 8) | lCurrentArray.at(1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double 
I2CSFPSlave::read_current() const {
    auto lCurrentRaw = this->read_current_raw();
    auto lTempCalibPair = read_calibration_parameter_pair(0x0);
    return ((lCurrentRaw*lTempCalibPair.first) + lTempCalibPair.second)*0.002;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
I2CSFPSlave::read_vendor_name() const {
    sfp_reachable();

    std::stringstream lVendorName;
    auto lVendorNameCharacters = this->read_i2cArray(0x14, 0x10);
    for (auto it = lVendorNameCharacters.begin(); it != lVendorNameCharacters.end(); ++it) {
        lVendorName << *it;
    }
    return lVendorName.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
I2CSFPSlave::read_vendor_part_number() const {
    sfp_reachable();

    std::stringstream lVendorPN;
    auto lVendorPNCharacters = this->read_i2cArray(0x28, 0x10);
    for (auto it = lVendorPNCharacters.begin(); it != lVendorPNCharacters.end(); ++it) {
        lVendorPN << *it;
    }
    return lVendorPN.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
I2CSFPSlave::read_serial_number() const {
    sfp_reachable();

    std::stringstream lSerialNumber;
    auto lSerialNumberCharacters = this->read_i2cArray(0x44, 0x10);
    for (auto it = lSerialNumberCharacters.begin(); it != lSerialNumberCharacters.end(); ++it) {
        lSerialNumber << *it;
    }
    return lSerialNumber.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CSFPSlave::read_ddm_support_bit() const {
    sfp_reachable();

    // Bit 6 of reg 5C tells us whether the SFP supports digital diagnostic monitoring (DDM)
    auto lDDMInfoByte = this->read_i2c(0x5C);
    return lDDMInfoByte & 0x40;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CSFPSlave::read_soft_tx_control_support_bit() const {
    sfp_reachable();

    // Bit 6 of reg 5d tells us whether the soft tx control is implemented in this sfp
    auto lEnhancedOptionsByte = this->read_i2c(0x5d);
    return lEnhancedOptionsByte & 0x40;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CSFPSlave::read_soft_tx_control_state() const {
    ddm_available();
    auto lOptStatusCtrlByte = this->read_i2c(0x51, 0x6e);
    // Bit 6 tells us the state of the soft tx_disble register
    return lOptStatusCtrlByte & 0x40;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CSFPSlave::read_tx_disable_pin_state() const {
    ddm_available();
    auto lOptStatusCtrlByte = this->read_i2c(0x51, 0x6e);
    // Bit 7 tells us the state of tx_disble pin
    return lOptStatusCtrlByte & 0x80;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool
I2CSFPSlave::read_i2c_reg_addressSwapBit() const {
    sfp_reachable();

    auto lDDMInfoByte = this->read_i2c(0x5C);
    // Bit 2 of byte 5C tells us whether special I2C address change operations are needed to access the DDM area
    return lDDMInfoByte & 0x4;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CSFPSlave::switch_soft_tx_control_bit(bool turn_on) const {
    ddm_available();

    if (!read_soft_tx_control_support_bit()) {
        throw SoftTxLaserControlUnsupported(ERS_HERE, "I2CSFPSlave", get_master_id());
    }
    
    // Get optional status/control bits
    auto lOptStatusCtrlByte = this->read_i2c(0x51, 0x6e);

    uint8_t lNewOptStatusCtrlByte;
    // Bit 6 of byte 0x6e controls the soft tx_disable
    if (turn_on) {
        lNewOptStatusCtrlByte = lOptStatusCtrlByte & ~(1UL << 6);     
    } else {
        lNewOptStatusCtrlByte = lOptStatusCtrlByte | (1UL << 6);
    }
    this->write_i2c(0x51,0x6e,lNewOptStatusCtrlByte);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
I2CSFPSlave::get_status(bool print_out) const {
    sfp_reachable();
    
    std::stringstream lStatus;
    std::vector<std::pair<std::string, std::string>> lSFPInfo;

    // Vendor name
    lSFPInfo.push_back(std::make_pair("Vendor", read_vendor_name()));
     
    // Vendor part number
    lSFPInfo.push_back(std::make_pair("Part number", read_vendor_part_number()));

    // Serial number
    lSFPInfo.push_back(std::make_pair("Serial number", read_serial_number()));
    
    // Does the SFP support DDM
    if (!read_ddm_support_bit()) {
        TLOG() << "DDM not available for SFP on I2C bus: " << get_master_id();
        lStatus << format_reg_table(lSFPInfo, "SFP status", {"", ""});
        if (print_out) std::cout << lStatus.str();
        return lStatus.str();
    } else {
        if (read_i2c_reg_addressSwapBit()) {
            TLOG() << "SFP DDM I2C address swap not supported. SFP on I2C bus: " << get_master_id();
            lStatus << format_reg_table(lSFPInfo, "SFP status", {"", ""});
            if (print_out) std::cout << lStatus.str();
            return lStatus.str();
        }
    }

    std::stringstream lTempStream;
    lTempStream << std::dec << std::fixed << std::setprecision(2) << read_temperature() << " C";
    lSFPInfo.push_back(std::make_pair("Temperature", lTempStream.str()));
    
    std::stringstream lVoltageStream;
    lVoltageStream << std::dec << std::fixed << std::setprecision(2) << read_voltage() << " V";
    lSFPInfo.push_back(std::make_pair("Supply voltage", lVoltageStream.str()));

    std::stringstream lRxPowerStream;
    lRxPowerStream << std::dec << std::fixed << std::setprecision(2) << read_rx_ower() << " uW";
    lSFPInfo.push_back(std::make_pair("Rx power", lRxPowerStream.str()));

    std::stringstream lTxPowerStream;
    lTxPowerStream << std::dec << std::fixed << std::setprecision(2) << read_tx_power() << " uW";
    lSFPInfo.push_back(std::make_pair("Tx power", lTxPowerStream.str()));

    std::stringstream lCurrentStream;
    lCurrentStream << std::dec << std::fixed << std::setprecision(2) << read_current() << " uA";
    lSFPInfo.push_back(std::make_pair("Tx current", lCurrentStream.str()));

    if (read_soft_tx_control_support_bit()) {
        //lSFPInfo.push_back(std::make_pair("Soft Tx disbale supported",  "True"));
        lSFPInfo.push_back(std::make_pair("Tx disable bit" , std::to_string(read_soft_tx_control_state())));
    } else {
       lSFPInfo.push_back(std::make_pair("Soft Tx disbale supported",  "False"));
    }

    lSFPInfo.push_back(std::make_pair("Tx disable pin", std::to_string(read_tx_disable_pin_state())));

    lStatus << format_reg_table(lSFPInfo, "SFP status", {"", ""});
    if (print_out) std::cout << lStatus.str();
    return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
I2CSFPSlave::get_info(timingmon::TimingSFPMonitorData& mon_data) const {
    sfp_reachable();
    
    // TODO ?
    //fault

    // Vendor name
    mon_data.vendor_name = this->read_vendor_name();
     
    // Vendor part number
    mon_data.vendor_pn = this->read_vendor_part_number();

    // Serial number TP DO?
    //lSFPInfo.push_back(std::make_pair("Serial number", read_serial_number()));
    
    // Does the SFP support DDM
    if (!this->read_ddm_support_bit()) {
        TLOG() << "DDM not available for SFP on I2C bus: " << get_master_id();
        mon_data.ddm_supported = 0;
        return;
    } else {
        if (this->read_i2c_reg_addressSwapBit()) {
            TLOG() << "SFP DDM I2C address swap not supported. SFP on I2C bus: " << get_master_id();
            return;
        }
    }

    mon_data.temperature = this->read_temperature();
    
    mon_data.supply_voltage = this->read_voltage();

    mon_data.rx_power = this->read_rx_ower();

    mon_data.tx_power = this->read_tx_power();

    mon_data.laser_current = this->read_current();

    mon_data.tx_disable_sw_supported = this->read_soft_tx_control_support_bit();

    mon_data.tx_disable_sw = this->read_soft_tx_control_state();
    
    mon_data.tx_disable_hw = this->read_tx_disable_pin_state();
}
//-----------------------------------------------------------------------------

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(I2CSFPNode)

//-----------------------------------------------------------------------------
I2CSFPNode::I2CSFPNode( const uhal::Node& node ) : I2CMasterNode(node), I2CSFPSlave(this, this->get_slave_address("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CSFPNode::I2CSFPNode( const I2CSFPNode& node ) : I2CMasterNode(node), I2CSFPSlave(this, this->get_slave_address("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CSFPNode::~I2CSFPNode() {
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq