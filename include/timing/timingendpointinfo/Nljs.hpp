/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains functions struct and other type definitions for shema in 
 * namespace dunedaq::timing::timingendpointinfo to be serialized via nlohmann::json.
 */
#ifndef DUNEDAQ_TIMING_TIMINGENDPOINTINFO_NLJS_HPP
#define DUNEDAQ_TIMING_TIMINGENDPOINTINFO_NLJS_HPP

// My structs
#include "timing/timingendpointinfo/Structs.hpp"


#include <nlohmann/json.hpp>

namespace dunedaq::timing::timingendpointinfo {

    using data_t = nlohmann::json;
    
    inline void to_json(data_t& j, const TimingEndpointInfo& obj) {
        j["state"] = obj.state;
        j["ready"] = obj.ready;
        j["partition"] = obj.partition;
        j["address"] = obj.address;
        j["timestamp"] = obj.timestamp;
        j["in_run"] = obj.in_run;
        j["in_spill"] = obj.in_spill;
        j["buffer_warning"] = obj.buffer_warning;
        j["buffer_error"] = obj.buffer_error;
        j["buffer_occupancy"] = obj.buffer_occupancy;
        j["event_counter"] = obj.event_counter;
        j["reset_out"] = obj.reset_out;
        j["sfp_tx_disable"] = obj.sfp_tx_disable;
        j["coarse_delay"] = obj.coarse_delay;
        j["fine_delay"] = obj.fine_delay;
    }
    
    inline void from_json(const data_t& j, TimingEndpointInfo& obj) {
        if (j.contains("state"))
            j.at("state").get_to(obj.state);    
        if (j.contains("ready"))
            j.at("ready").get_to(obj.ready);    
        if (j.contains("partition"))
            j.at("partition").get_to(obj.partition);    
        if (j.contains("address"))
            j.at("address").get_to(obj.address);    
        if (j.contains("timestamp"))
            j.at("timestamp").get_to(obj.timestamp);    
        if (j.contains("in_run"))
            j.at("in_run").get_to(obj.in_run);    
        if (j.contains("in_spill"))
            j.at("in_spill").get_to(obj.in_spill);    
        if (j.contains("buffer_warning"))
            j.at("buffer_warning").get_to(obj.buffer_warning);    
        if (j.contains("buffer_error"))
            j.at("buffer_error").get_to(obj.buffer_error);    
        if (j.contains("buffer_occupancy"))
            j.at("buffer_occupancy").get_to(obj.buffer_occupancy);    
        if (j.contains("event_counter"))
            j.at("event_counter").get_to(obj.event_counter);    
        if (j.contains("reset_out"))
            j.at("reset_out").get_to(obj.reset_out);    
        if (j.contains("sfp_tx_disable"))
            j.at("sfp_tx_disable").get_to(obj.sfp_tx_disable);    
        if (j.contains("coarse_delay"))
            j.at("coarse_delay").get_to(obj.coarse_delay);    
        if (j.contains("fine_delay"))
            j.at("fine_delay").get_to(obj.fine_delay);    
    }
    
    inline void to_json(data_t& j, const TimingFLCmdCounters& obj) {
        j["TimeSync"] = obj.TimeSync;
        j["Echo"] = obj.Echo;
        j["SpillStart"] = obj.SpillStart;
        j["SpillStop"] = obj.SpillStop;
        j["RunStart"] = obj.RunStart;
        j["RunStop"] = obj.RunStop;
        j["WibCalib"] = obj.WibCalib;
        j["SSPCalib"] = obj.SSPCalib;
        j["FakeTrig0"] = obj.FakeTrig0;
        j["FakeTrig1"] = obj.FakeTrig1;
        j["FakeTrig2"] = obj.FakeTrig2;
        j["FakeTrig3"] = obj.FakeTrig3;
        j["BeamTrig"] = obj.BeamTrig;
        j["NoBeamTrig"] = obj.NoBeamTrig;
        j["ExtFakeTrig"] = obj.ExtFakeTrig;
    }
    
    inline void from_json(const data_t& j, TimingFLCmdCounters& obj) {
        if (j.contains("TimeSync"))
            j.at("TimeSync").get_to(obj.TimeSync);    
        if (j.contains("Echo"))
            j.at("Echo").get_to(obj.Echo);    
        if (j.contains("SpillStart"))
            j.at("SpillStart").get_to(obj.SpillStart);    
        if (j.contains("SpillStop"))
            j.at("SpillStop").get_to(obj.SpillStop);    
        if (j.contains("RunStart"))
            j.at("RunStart").get_to(obj.RunStart);    
        if (j.contains("RunStop"))
            j.at("RunStop").get_to(obj.RunStop);    
        if (j.contains("WibCalib"))
            j.at("WibCalib").get_to(obj.WibCalib);    
        if (j.contains("SSPCalib"))
            j.at("SSPCalib").get_to(obj.SSPCalib);    
        if (j.contains("FakeTrig0"))
            j.at("FakeTrig0").get_to(obj.FakeTrig0);    
        if (j.contains("FakeTrig1"))
            j.at("FakeTrig1").get_to(obj.FakeTrig1);    
        if (j.contains("FakeTrig2"))
            j.at("FakeTrig2").get_to(obj.FakeTrig2);    
        if (j.contains("FakeTrig3"))
            j.at("FakeTrig3").get_to(obj.FakeTrig3);    
        if (j.contains("BeamTrig"))
            j.at("BeamTrig").get_to(obj.BeamTrig);    
        if (j.contains("NoBeamTrig"))
            j.at("NoBeamTrig").get_to(obj.NoBeamTrig);    
        if (j.contains("ExtFakeTrig"))
            j.at("ExtFakeTrig").get_to(obj.ExtFakeTrig);    
    }
    
} // namespace dunedaq::timing::timingendpointinfo

#endif // DUNEDAQ_TIMING_TIMINGENDPOINTINFO_NLJS_HPP