/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains struct and other type definitions for shema in 
 * namespace dunedaq::timing::timingendpointinfo.
 */
#ifndef DUNEDAQ_TIMING_TIMINGENDPOINTINFO_STRUCTS_HPP
#define DUNEDAQ_TIMING_TIMINGENDPOINTINFO_STRUCTS_HPP

#include <cstdint>

#include <string>

namespace dunedaq::timing::timingendpointinfo {

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

    // @brief Timing endpoint monitor data
    struct TimingEndpointInfo 
    {

        // @brief Endpoint state
        RegValue state = 0;

        // @brief Endpoint ready flag
        BoolData ready = false;

        // @brief Endpoint partition
        RegValue partition = 0;

        // @brief Endpoint address
        RegValue address = 0;

        // @brief Endpoint timestamp
        LongUint timestamp = 0;

        // @brief Endpoint in run flag
        BoolData in_run = false;

        // @brief Endpoint in spill flag
        BoolData in_spill = false;

        // @brief Buffer warning flag
        BoolData buffer_warning = false;

        // @brief Buffer error flag
        BoolData buffer_error = false;

        // @brief Buffer occupancy
        RegValue buffer_occupancy = 0;

        // @brief Event counter
        RegValue event_counter = 0;

        // @brief Endpoint out reset line
        BoolData reset_out = false;

        // @brief Endpoint SFP tx disable line
        BoolData sfp_tx_disable = false;

        // @brief Configured coarse delay
        RegValue coarse_delay = 0;

        // @brief Configured fine delay
        RegValue fine_delay = 0;
    };

    // @brief Command counters list
    struct TimingFLCmdCounters 
    {

        // @brief TimeSync counters
        RegValue TimeSync = 0;

        // @brief Echo counters
        RegValue Echo = 0;

        // @brief SpillStart counters
        RegValue SpillStart = 0;

        // @brief SpillStop counters
        RegValue SpillStop = 0;

        // @brief RunStart counters
        RegValue RunStart = 0;

        // @brief RunStop counters
        RegValue RunStop = 0;

        // @brief WibCalib counters
        RegValue WibCalib = 0;

        // @brief SSPCalib counters
        RegValue SSPCalib = 0;

        // @brief FakeTrig0 counters
        RegValue FakeTrig0 = 0;

        // @brief FakeTrig1 counters
        RegValue FakeTrig1 = 0;

        // @brief FakeTrig2 counters
        RegValue FakeTrig2 = 0;

        // @brief FakeTrig3 counters
        RegValue FakeTrig3 = 0;

        // @brief BeamTrig counters
        RegValue BeamTrig = 0;

        // @brief NoBeamTrig counters
        RegValue NoBeamTrig = 0;

        // @brief ExtFakeTrig counters
        RegValue ExtFakeTrig = 0;
    };

} // namespace dunedaq::timing::timingendpointinfo

#endif // DUNEDAQ_TIMING_TIMINGENDPOINTINFO_STRUCTS_HPP