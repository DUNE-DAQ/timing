/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains functions struct and other type definitions for shema in 
 * namespace dunedaq::timing::timingfirmwareinfo to be serialized via nlohmann::json.
 */
#ifndef DUNEDAQ_TIMING_TIMINGFIRMWAREINFO_NLJS_HPP
#define DUNEDAQ_TIMING_TIMINGFIRMWAREINFO_NLJS_HPP

// My structs
#include "timing/timingfirmwareinfo/Structs.hpp"

// Nljs for externally referenced schema
#include "timing/timingendpointinfo/Nljs.hpp"
#include "timing/timinghardwareinfo/Nljs.hpp"

#include <nlohmann/json.hpp>

namespace dunedaq::timing::timingfirmwareinfo {

    using data_t = nlohmann::json;
    
    inline void to_json(data_t& j, const HSIFirmwareMonitorData& obj) {
        j["source"] = obj.source;
        j["re_mask"] = obj.re_mask;
        j["fe_mask"] = obj.fe_mask;
        j["inv_mask"] = obj.inv_mask;
        j["buffer_enabled"] = obj.buffer_enabled;
        j["buffer_error"] = obj.buffer_error;
        j["buffer_warning"] = obj.buffer_warning;
        j["buffer_occupancy"] = obj.buffer_occupancy;
        j["enabled"] = obj.enabled;
    }
    
    inline void from_json(const data_t& j, HSIFirmwareMonitorData& obj) {
        if (j.contains("source"))
            j.at("source").get_to(obj.source);    
        if (j.contains("re_mask"))
            j.at("re_mask").get_to(obj.re_mask);    
        if (j.contains("fe_mask"))
            j.at("fe_mask").get_to(obj.fe_mask);    
        if (j.contains("inv_mask"))
            j.at("inv_mask").get_to(obj.inv_mask);    
        if (j.contains("buffer_enabled"))
            j.at("buffer_enabled").get_to(obj.buffer_enabled);    
        if (j.contains("buffer_error"))
            j.at("buffer_error").get_to(obj.buffer_error);    
        if (j.contains("buffer_warning"))
            j.at("buffer_warning").get_to(obj.buffer_warning);    
        if (j.contains("buffer_occupancy"))
            j.at("buffer_occupancy").get_to(obj.buffer_occupancy);    
        if (j.contains("enabled"))
            j.at("enabled").get_to(obj.enabled);    
    }
    
    inline void to_json(data_t& j, const MasterMonitorData& obj) {
        j["timestamp"] = obj.timestamp;
        j["ts_bcast_enable"] = obj.ts_bcast_enable;
        j["ts_valid"] = obj.ts_valid;
        j["ts_tx_err"] = obj.ts_tx_err;
        j["tx_err"] = obj.tx_err;
        j["ctrs_rdy"] = obj.ctrs_rdy;
    }
    
    inline void from_json(const data_t& j, MasterMonitorData& obj) {
        if (j.contains("timestamp"))
            j.at("timestamp").get_to(obj.timestamp);    
        if (j.contains("ts_bcast_enable"))
            j.at("ts_bcast_enable").get_to(obj.ts_bcast_enable);    
        if (j.contains("ts_valid"))
            j.at("ts_valid").get_to(obj.ts_valid);    
        if (j.contains("ts_tx_err"))
            j.at("ts_tx_err").get_to(obj.ts_tx_err);    
        if (j.contains("tx_err"))
            j.at("tx_err").get_to(obj.tx_err);    
        if (j.contains("ctrs_rdy"))
            j.at("ctrs_rdy").get_to(obj.ctrs_rdy);    
    }
    
    inline void to_json(data_t& j, const SentCommandCounter& obj) {
        j["counts"] = obj.counts;
    }
    
    inline void from_json(const data_t& j, SentCommandCounter& obj) {
        if (j.contains("counts"))
            j.at("counts").get_to(obj.counts);    
    }
    
    inline void to_json(data_t& j, const TimingDeviceInfo& obj) {
        j["device"] = obj.device;
        j["pll_info"] = obj.pll_info;
        j["master_info"] = obj.master_info;
        j["endpoint_info"] = obj.endpoint_info;
        j["hsi_info"] = obj.hsi_info;
    }
    
    inline void from_json(const data_t& j, TimingDeviceInfo& obj) {
        if (j.contains("device"))
            j.at("device").get_to(obj.device);    
        if (j.contains("pll_info"))
            j.at("pll_info").get_to(obj.pll_info);    
        if (j.contains("master_info"))
            j.at("master_info").get_to(obj.master_info);    
        if (j.contains("endpoint_info"))
            j.at("endpoint_info").get_to(obj.endpoint_info);    
        if (j.contains("hsi_info"))
            j.at("hsi_info").get_to(obj.hsi_info);    
    }
    
    inline void to_json(data_t& j, const TimingFLCmdCounter& obj) {
        j["accepted"] = obj.accepted;
        j["rejected"] = obj.rejected;
    }
    
    inline void from_json(const data_t& j, TimingFLCmdCounter& obj) {
        if (j.contains("accepted"))
            j.at("accepted").get_to(obj.accepted);    
        if (j.contains("rejected"))
            j.at("rejected").get_to(obj.rejected);    
    }
    
} // namespace dunedaq::timing::timingfirmwareinfo

#endif // DUNEDAQ_TIMING_TIMINGFIRMWAREINFO_NLJS_HPP