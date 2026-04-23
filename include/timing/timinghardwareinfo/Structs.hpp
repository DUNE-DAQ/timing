/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains struct and other type definitions for shema in 
 * namespace dunedaq::timing::timinghardwareinfo.
 */
#ifndef DUNEDAQ_TIMING_TIMINGHARDWAREINFO_STRUCTS_HPP
#define DUNEDAQ_TIMING_TIMINGHARDWAREINFO_STRUCTS_HPP

#include <cstdint>

#include <string>

namespace dunedaq::timing::timinghardwareinfo {

    // @brief A bool
    using BoolData = bool;

    // @brief A double
    using DoubleValue = double;


    // @brief 64 bit uint
    using LongInt = int64_t;


    // @brief 64 bit uint
    using LongUint = uint64_t; // NOLINT


    // @brief 32 bit uint
    using RegValue = uint32_t; // NOLINT


    // @brief A string field
    using TextData = std::string;

    // @brief Timing FIB monitor data
    struct TimingFIBMonitorData 
    {

        // @brief Active SFP MUX channel
        RegValue active_sfp_mux = 0;

        // @brief MMCM OK flag
        BoolData mmcm_ok = false;

        // @brief Sticky MMCM OK flag
        BoolData mmcm_sticky = false;

        // @brief PLL OK flag
        BoolData pll_ok = false;

        // @brief Sticky PLL OK flag
        BoolData pll_sticky = false;
    };

    // @brief Timing FMC monitor data
    struct TimingFMCMonitorData 
    {

        // @brief CDR LOL flag
        BoolData cdr_lol = false;

        // @brief CDR LOS flag
        BoolData cdr_los = false;

        // @brief MMCM OK flag
        BoolData mmcm_ok = false;

        // @brief Sticky MMCM OK flag
        BoolData mmcm_sticky = false;

        // @brief SFP fault pin value
        BoolData sfp_flt = false;

        // @brief SFP LOS pin value
        BoolData sfp_los = false;

        // @brief CDR frequency
        DoubleValue cdr_freq = 0.0;

        // @brief PLL frequency
        DoubleValue pll_freq = 0.0;
    };

    // @brief Timing FMC monitor data
    struct TimingMIBMonitorData 
    {

        // @brief CDR 0 LOL flag
        BoolData cdr_0_lol = false;

        // @brief CDR 0 LOS flag
        BoolData cdr_0_los = false;

        // @brief CDR 1 LOL flag
        BoolData cdr_1_lol = false;

        // @brief CDR 1 LOS flag
        BoolData cdr_1_los = false;

        // @brief MMCM OK flag
        BoolData mmcm_ok = false;

        // @brief Sticky MMCM OK flag
        BoolData mmcm_sticky = false;

        // @brief SFP 0 fault pin value
        BoolData sfp_0_flt = false;

        // @brief SFP 0 LOS pin value
        BoolData sfp_0_los = false;

        // @brief SFP 1 fault pin value
        BoolData sfp_1_flt = false;

        // @brief SFP 1 LOS pin value
        BoolData sfp_1_los = false;

        // @brief SFP 2 fault pin value
        BoolData sfp_2_flt = false;

        // @brief SFP 2 LOS pin value
        BoolData sfp_2_los = false;

        // @brief CDR 0 frequency
        DoubleValue cdr_0_freq = 0.0;

        // @brief CDR 1 frequency
        DoubleValue cdr_1_freq = 0.0;

        // @brief PLL frequency
        DoubleValue pll_freq = 0.0;
    };

    // @brief Timing PC059 monitor data
    struct TimingPC059MonitorData 
    {

        // @brief CDR LOL flag
        BoolData cdr_lol = false;

        // @brief CDR LOS flag
        BoolData cdr_los = false;

        // @brief MMCM OK flag
        BoolData mmcm_ok = false;

        // @brief Sticky MMCM OK flag
        BoolData mmcm_sticky = false;

        // @brief PLL LOL flag
        BoolData pll_lol = false;

        // @brief PLL OK flag
        BoolData pll_ok = false;

        // @brief Sticky PLL OK flag
        BoolData pll_sticky = false;

        // @brief SFP LOS pin value
        RegValue sfp_los = 0;

        // @brief Upstream CDR LOL flag
        BoolData ucdr_lol = false;

        // @brief Upstream CDR LOS flag
        BoolData ucdr_los = false;

        // @brief Upstream SFP fault pin value
        BoolData usfp_flt = false;

        // @brief Upstream SFP LOS pin value
        BoolData usfp_los = false;

        // @brief Active SFP MUX channel
        RegValue active_sfp_mux = 0;
    };

    // @brief Timing PLL monitor structure for data read over I2C
    struct TimingPLLMonitorData 
    {

        // @brief PLL config ID
        TextData config_id = "";

        // @brief Cal pll
        BoolData cal_pll = false;

        // @brief Holdover flag
        BoolData hold = false;

        // @brief Loss of lock flag
        BoolData lol = false;

        // @brief Loss of signal flag
        RegValue los = 0;

        // @brief Loss of signal flag XAXB
        RegValue los_xaxb = 0;

        // @brief Loss of signal flag XAXB stricky
        RegValue los_xaxb_flg = 0;

        // @brief Out of frequency flags
        RegValue oof = 0;

        // @brief Out of frequency flags sticky
        RegValue oof_sticky = 0;

        // @brief SMBUS timeout
        BoolData smbus_timeout = false;

        // @brief SMBUS timeout sticky
        BoolData smbus_timeout_flg = false;

        // @brief In calibration flag
        BoolData sys_in_cal = false;

        // @brief In calibration flag sticky
        BoolData sys_in_cal_flg = false;

        // @brief XA-XB error flag
        BoolData xaxb_err = false;

        // @brief XA-XB error flag sticky
        BoolData xaxb_err_flg = false;
    };

    // @brief Timing SFP monitor structure for data read over I2C
    struct TimingSFPMonitorData 
    {

        // @brief Vendor name
        TextData vendor_name = "";

        // @brief Vendor part number
        TextData vendor_pn = "";

        // @brief SFP fault flag
        BoolData sfp_fault = false;

        // @brief DDM supported flag
        BoolData ddm_supported = false;

        // @brief Hardware transmit disable pin value
        BoolData tx_disable_hw = false;

        // @brief Soft transmit disable bit control supported
        BoolData tx_disable_sw_supported = false;

        // @brief Soft transmit disable bit value
        BoolData tx_disable_sw = false;

        // @brief SFP temperature
        DoubleValue temperature = 0.0;

        // @brief SFP supply voltage
        DoubleValue supply_voltage = 0.0;

        // @brief SFP receive power
        DoubleValue rx_power = 0.0;

        // @brief SFP transmit power
        DoubleValue tx_power = 0.0;

        // @brief SFP laser current
        DoubleValue laser_current = 0.0;

        // @brief Is the data valid?
        BoolData data_valid = false;
    };

    // @brief Timing TLU monitor data
    struct TimingTLUMonitorData 
    {

        // @brief CDR LOL flag
        BoolData cdr_lol = false;

        // @brief CDR LOS flag
        BoolData cdr_los = false;

        // @brief MMCM OK flag
        BoolData mmcm_ok = false;

        // @brief Sticky MMCM OK flag
        BoolData mmcm_sticky = false;

        // @brief PLL OK flag
        BoolData pll_ok = false;

        // @brief Sticky PLL OK flag
        BoolData pll_sticky = false;

        // @brief SFP fault pin value
        BoolData sfp_flt = false;

        // @brief SFP LOS pin value
        BoolData sfp_los = false;
    };

} // namespace dunedaq::timing::timinghardwareinfo

#endif // DUNEDAQ_TIMING_TIMINGHARDWAREINFO_STRUCTS_HPP