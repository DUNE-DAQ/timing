/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains struct and other type definitions for shema in 
 * namespace dunedaq::timing::timingfirmware.
 */
#ifndef DUNEDAQ_TIMING_TIMINGFIRMWARE_STRUCTS_HPP
#define DUNEDAQ_TIMING_TIMINGFIRMWARE_STRUCTS_HPP

#include <cstdint>

#include <vector>
#include <string>

namespace dunedaq::timing::timingfirmware {

    // @brief A bool
    using BoolData = bool;

    // @brief A double
    using DoubleValue = double;


    // @brief 32 bit uint
    using RegValue = uint32_t; // NOLINT


    // @brief integer
    using IntData = int32_t;


    // @brief Endpoint check result data
    struct EndpointCheckResult 
    {

        // @brief Address of the checked endpoint
        RegValue address = 0;

        // @brief Was the endpoint alive?
        BoolData alive = false;

        // @brief Measured endpoint round trip time
        IntData round_trip_time = -1;

        // @brief State of the checked endpoint
        IntData state = -1;

        // @brief Measured endpoint round trip time after delay apply
        IntData round_trip_time_after_delay_apply = -1;

        // @brief State of the checked endpoint after delays applied
        IntData state_after_delay_apply = -1;

        // @brief Applied delay
        IntData applied_delay = -1;
    };

    // @brief A vector timing endpoint check result data
    using EndpointCheckResultoVector = std::vector<dunedaq::timing::timingfirmware::EndpointCheckResult>;

    // @brief 64 bit int
    using LongInt = int64_t;


    // @brief 64 bit uint
    using LongUint = uint64_t; // NOLINT


    // @brief A string field
    using TextData = std::string;

} // namespace dunedaq::timing::timingfirmware

#endif // DUNEDAQ_TIMING_TIMINGFIRMWARE_STRUCTS_HPP