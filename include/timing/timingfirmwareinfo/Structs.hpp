/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains struct and other type definitions for shema in 
 * namespace dunedaq::timing::timingfirmwareinfo.
 */
#ifndef DUNEDAQ_TIMING_TIMINGFIRMWAREINFO_STRUCTS_HPP
#define DUNEDAQ_TIMING_TIMINGFIRMWAREINFO_STRUCTS_HPP

#include <cstdint>
#include "timing/timingendpointinfo/Structs.hpp"
#include "timing/timinghardwareinfo/Structs.hpp"

#include <string>

namespace dunedaq::timing::timingfirmwareinfo {

    // @brief A bool
    using BoolData = bool;

    // @brief A double
    using DoubleValue = double;


    // @brief 32 bit uint
    using RegValue = uint32_t; // NOLINT


    // @brief HSI monitor data
    struct HSIFirmwareMonitorData 
    {

        // @brief Source of HSI data
        RegValue source = 0;

        // @brief Rising edge mask
        RegValue re_mask = 0;

        // @brief Falling edge mask
        RegValue fe_mask = 0;

        // @brief Inverted mask
        RegValue inv_mask = 0;

        // @brief Buffer enable flag
        BoolData buffer_enabled = false;

        // @brief Buffer enable flag
        BoolData buffer_error = false;

        // @brief Buffer enable flag
        BoolData buffer_warning = false;

        // @brief Number of words in buffer
        RegValue buffer_occupancy = 0;

        // @brief HSI triggering enabled
        BoolData enabled = false;
    };

    // @brief 64 bit uint
    using LongInt = int64_t;


    // @brief 64 bit uint
    using LongUint = uint64_t; // NOLINT


    // @brief master monitor data
    struct MasterMonitorData 
    {

        // @brief Timestamp
        LongUint timestamp = 0;

        // @brief Timestamp bcast enable
        RegValue ts_bcast_enable = 0;

        // @brief Timestamp valid
        RegValue ts_valid = 0;

        // @brief Timestamp transmit error
        RegValue ts_tx_err = 0;

        // @brief Tx error
        RegValue tx_err = 0;

        // @brief Counters ready
        RegValue ctrs_rdy = 0;
    };

    // @brief Sent command counters structure
    struct SentCommandCounter 
    {

        // @brief Number of commands sent
        RegValue counts = 0;
    };

    // @brief A string field
    using TextData = std::string;

    // @brief Timing hw cmd structure
    struct TimingDeviceInfo 
    {

        // @brief Device name
        TextData device = "";

        // @brief IO info payload
        timinghardwareinfo::TimingPLLMonitorData pll_info = {};

        // @brief Master info payload
        MasterMonitorData master_info = {};

        // @brief Endpoint info payload
        timingendpointinfo::TimingEndpointInfo endpoint_info = {};

        // @brief HSI info payload
        HSIFirmwareMonitorData hsi_info = {};
    };

    // @brief Fixed length command counters structure
    struct TimingFLCmdCounter 
    {

        // @brief Number of commands accepted
        RegValue accepted = 0;

        // @brief Number of commands rejected
        RegValue rejected = 0;
    };

} // namespace dunedaq::timing::timingfirmwareinfo

#endif // DUNEDAQ_TIMING_TIMINGFIRMWAREINFO_STRUCTS_HPP