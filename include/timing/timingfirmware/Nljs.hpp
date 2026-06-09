/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains functions struct and other type definitions for shema in 
 * namespace dunedaq::timing::timingfirmware to be serialized via nlohmann::json.
 */
#ifndef DUNEDAQ_TIMING_TIMINGFIRMWARE_NLJS_HPP
#define DUNEDAQ_TIMING_TIMINGFIRMWARE_NLJS_HPP

// My structs
#include "timing/timingfirmware/Structs.hpp"


#include <nlohmann/json.hpp>

namespace dunedaq::timing::timingfirmware {

    using data_t = nlohmann::json;
    
    inline void to_json(data_t& j, const EndpointCheckResult& obj) {
        j["address"] = obj.address;
        j["alive"] = obj.alive;
        j["round_trip_time"] = obj.round_trip_time;
        j["state"] = obj.state;
        j["round_trip_time_after_delay_apply"] = obj.round_trip_time_after_delay_apply;
        j["state_after_delay_apply"] = obj.state_after_delay_apply;
        j["applied_delay"] = obj.applied_delay;
    }
    
    inline void from_json(const data_t& j, EndpointCheckResult& obj) {
        if (j.contains("address"))
            j.at("address").get_to(obj.address);    
        if (j.contains("alive"))
            j.at("alive").get_to(obj.alive);    
        if (j.contains("round_trip_time"))
            j.at("round_trip_time").get_to(obj.round_trip_time);    
        if (j.contains("state"))
            j.at("state").get_to(obj.state);    
        if (j.contains("round_trip_time_after_delay_apply"))
            j.at("round_trip_time_after_delay_apply").get_to(obj.round_trip_time_after_delay_apply);    
        if (j.contains("state_after_delay_apply"))
            j.at("state_after_delay_apply").get_to(obj.state_after_delay_apply);    
        if (j.contains("applied_delay"))
            j.at("applied_delay").get_to(obj.applied_delay);    
    }
    
} // namespace dunedaq::timing::timingfirmware

#endif // DUNEDAQ_TIMING_TIMINGFIRMWARE_NLJS_HPP