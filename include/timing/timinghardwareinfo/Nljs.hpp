/*
 * This file is 100% generated.  Any manual edits will likely be lost.
 *
 * This contains functions struct and other type definitions for shema in 
 * namespace dunedaq::timing::timinghardwareinfo to be serialized via nlohmann::json.
 */
#ifndef DUNEDAQ_TIMING_TIMINGHARDWAREINFO_NLJS_HPP
#define DUNEDAQ_TIMING_TIMINGHARDWAREINFO_NLJS_HPP

// My structs
#include "timing/timinghardwareinfo/Structs.hpp"


#include <nlohmann/json.hpp>

namespace dunedaq::timing::timinghardwareinfo {

    using data_t = nlohmann::json;
    
    inline void to_json(data_t& j, const TimingFIBMonitorData& obj) {
        j["active_sfp_mux"] = obj.active_sfp_mux;
        j["mmcm_ok"] = obj.mmcm_ok;
        j["mmcm_sticky"] = obj.mmcm_sticky;
        j["pll_ok"] = obj.pll_ok;
        j["pll_sticky"] = obj.pll_sticky;
    }
    
    inline void from_json(const data_t& j, TimingFIBMonitorData& obj) {
        if (j.contains("active_sfp_mux"))
            j.at("active_sfp_mux").get_to(obj.active_sfp_mux);    
        if (j.contains("mmcm_ok"))
            j.at("mmcm_ok").get_to(obj.mmcm_ok);    
        if (j.contains("mmcm_sticky"))
            j.at("mmcm_sticky").get_to(obj.mmcm_sticky);    
        if (j.contains("pll_ok"))
            j.at("pll_ok").get_to(obj.pll_ok);    
        if (j.contains("pll_sticky"))
            j.at("pll_sticky").get_to(obj.pll_sticky);    
    }
    
    inline void to_json(data_t& j, const TimingFMCMonitorData& obj) {
        j["cdr_lol"] = obj.cdr_lol;
        j["cdr_los"] = obj.cdr_los;
        j["mmcm_ok"] = obj.mmcm_ok;
        j["mmcm_sticky"] = obj.mmcm_sticky;
        j["sfp_flt"] = obj.sfp_flt;
        j["sfp_los"] = obj.sfp_los;
        j["cdr_freq"] = obj.cdr_freq;
        j["pll_freq"] = obj.pll_freq;
    }
    
    inline void from_json(const data_t& j, TimingFMCMonitorData& obj) {
        if (j.contains("cdr_lol"))
            j.at("cdr_lol").get_to(obj.cdr_lol);    
        if (j.contains("cdr_los"))
            j.at("cdr_los").get_to(obj.cdr_los);    
        if (j.contains("mmcm_ok"))
            j.at("mmcm_ok").get_to(obj.mmcm_ok);    
        if (j.contains("mmcm_sticky"))
            j.at("mmcm_sticky").get_to(obj.mmcm_sticky);    
        if (j.contains("sfp_flt"))
            j.at("sfp_flt").get_to(obj.sfp_flt);    
        if (j.contains("sfp_los"))
            j.at("sfp_los").get_to(obj.sfp_los);    
        if (j.contains("cdr_freq"))
            j.at("cdr_freq").get_to(obj.cdr_freq);    
        if (j.contains("pll_freq"))
            j.at("pll_freq").get_to(obj.pll_freq);    
    }
    
    inline void to_json(data_t& j, const TimingMIBMonitorData& obj) {
        j["cdr_0_lol"] = obj.cdr_0_lol;
        j["cdr_0_los"] = obj.cdr_0_los;
        j["cdr_1_lol"] = obj.cdr_1_lol;
        j["cdr_1_los"] = obj.cdr_1_los;
        j["mmcm_ok"] = obj.mmcm_ok;
        j["mmcm_sticky"] = obj.mmcm_sticky;
        j["sfp_0_flt"] = obj.sfp_0_flt;
        j["sfp_0_los"] = obj.sfp_0_los;
        j["sfp_1_flt"] = obj.sfp_1_flt;
        j["sfp_1_los"] = obj.sfp_1_los;
        j["sfp_2_flt"] = obj.sfp_2_flt;
        j["sfp_2_los"] = obj.sfp_2_los;
        j["cdr_0_freq"] = obj.cdr_0_freq;
        j["cdr_1_freq"] = obj.cdr_1_freq;
        j["pll_freq"] = obj.pll_freq;
    }
    
    inline void from_json(const data_t& j, TimingMIBMonitorData& obj) {
        if (j.contains("cdr_0_lol"))
            j.at("cdr_0_lol").get_to(obj.cdr_0_lol);    
        if (j.contains("cdr_0_los"))
            j.at("cdr_0_los").get_to(obj.cdr_0_los);    
        if (j.contains("cdr_1_lol"))
            j.at("cdr_1_lol").get_to(obj.cdr_1_lol);    
        if (j.contains("cdr_1_los"))
            j.at("cdr_1_los").get_to(obj.cdr_1_los);    
        if (j.contains("mmcm_ok"))
            j.at("mmcm_ok").get_to(obj.mmcm_ok);    
        if (j.contains("mmcm_sticky"))
            j.at("mmcm_sticky").get_to(obj.mmcm_sticky);    
        if (j.contains("sfp_0_flt"))
            j.at("sfp_0_flt").get_to(obj.sfp_0_flt);    
        if (j.contains("sfp_0_los"))
            j.at("sfp_0_los").get_to(obj.sfp_0_los);    
        if (j.contains("sfp_1_flt"))
            j.at("sfp_1_flt").get_to(obj.sfp_1_flt);    
        if (j.contains("sfp_1_los"))
            j.at("sfp_1_los").get_to(obj.sfp_1_los);    
        if (j.contains("sfp_2_flt"))
            j.at("sfp_2_flt").get_to(obj.sfp_2_flt);    
        if (j.contains("sfp_2_los"))
            j.at("sfp_2_los").get_to(obj.sfp_2_los);    
        if (j.contains("cdr_0_freq"))
            j.at("cdr_0_freq").get_to(obj.cdr_0_freq);    
        if (j.contains("cdr_1_freq"))
            j.at("cdr_1_freq").get_to(obj.cdr_1_freq);    
        if (j.contains("pll_freq"))
            j.at("pll_freq").get_to(obj.pll_freq);    
    }
    
    inline void to_json(data_t& j, const TimingPC059MonitorData& obj) {
        j["cdr_lol"] = obj.cdr_lol;
        j["cdr_los"] = obj.cdr_los;
        j["mmcm_ok"] = obj.mmcm_ok;
        j["mmcm_sticky"] = obj.mmcm_sticky;
        j["pll_lol"] = obj.pll_lol;
        j["pll_ok"] = obj.pll_ok;
        j["pll_sticky"] = obj.pll_sticky;
        j["sfp_los"] = obj.sfp_los;
        j["ucdr_lol"] = obj.ucdr_lol;
        j["ucdr_los"] = obj.ucdr_los;
        j["usfp_flt"] = obj.usfp_flt;
        j["usfp_los"] = obj.usfp_los;
        j["active_sfp_mux"] = obj.active_sfp_mux;
    }
    
    inline void from_json(const data_t& j, TimingPC059MonitorData& obj) {
        if (j.contains("cdr_lol"))
            j.at("cdr_lol").get_to(obj.cdr_lol);    
        if (j.contains("cdr_los"))
            j.at("cdr_los").get_to(obj.cdr_los);    
        if (j.contains("mmcm_ok"))
            j.at("mmcm_ok").get_to(obj.mmcm_ok);    
        if (j.contains("mmcm_sticky"))
            j.at("mmcm_sticky").get_to(obj.mmcm_sticky);    
        if (j.contains("pll_lol"))
            j.at("pll_lol").get_to(obj.pll_lol);    
        if (j.contains("pll_ok"))
            j.at("pll_ok").get_to(obj.pll_ok);    
        if (j.contains("pll_sticky"))
            j.at("pll_sticky").get_to(obj.pll_sticky);    
        if (j.contains("sfp_los"))
            j.at("sfp_los").get_to(obj.sfp_los);    
        if (j.contains("ucdr_lol"))
            j.at("ucdr_lol").get_to(obj.ucdr_lol);    
        if (j.contains("ucdr_los"))
            j.at("ucdr_los").get_to(obj.ucdr_los);    
        if (j.contains("usfp_flt"))
            j.at("usfp_flt").get_to(obj.usfp_flt);    
        if (j.contains("usfp_los"))
            j.at("usfp_los").get_to(obj.usfp_los);    
        if (j.contains("active_sfp_mux"))
            j.at("active_sfp_mux").get_to(obj.active_sfp_mux);    
    }
    
    inline void to_json(data_t& j, const TimingPLLMonitorData& obj) {
        j["config_id"] = obj.config_id;
        j["cal_pll"] = obj.cal_pll;
        j["hold"] = obj.hold;
        j["lol"] = obj.lol;
        j["los"] = obj.los;
        j["los_xaxb"] = obj.los_xaxb;
        j["los_xaxb_flg"] = obj.los_xaxb_flg;
        j["oof"] = obj.oof;
        j["oof_sticky"] = obj.oof_sticky;
        j["smbus_timeout"] = obj.smbus_timeout;
        j["smbus_timeout_flg"] = obj.smbus_timeout_flg;
        j["sys_in_cal"] = obj.sys_in_cal;
        j["sys_in_cal_flg"] = obj.sys_in_cal_flg;
        j["xaxb_err"] = obj.xaxb_err;
        j["xaxb_err_flg"] = obj.xaxb_err_flg;
    }
    
    inline void from_json(const data_t& j, TimingPLLMonitorData& obj) {
        if (j.contains("config_id"))
            j.at("config_id").get_to(obj.config_id);    
        if (j.contains("cal_pll"))
            j.at("cal_pll").get_to(obj.cal_pll);    
        if (j.contains("hold"))
            j.at("hold").get_to(obj.hold);    
        if (j.contains("lol"))
            j.at("lol").get_to(obj.lol);    
        if (j.contains("los"))
            j.at("los").get_to(obj.los);    
        if (j.contains("los_xaxb"))
            j.at("los_xaxb").get_to(obj.los_xaxb);    
        if (j.contains("los_xaxb_flg"))
            j.at("los_xaxb_flg").get_to(obj.los_xaxb_flg);    
        if (j.contains("oof"))
            j.at("oof").get_to(obj.oof);    
        if (j.contains("oof_sticky"))
            j.at("oof_sticky").get_to(obj.oof_sticky);    
        if (j.contains("smbus_timeout"))
            j.at("smbus_timeout").get_to(obj.smbus_timeout);    
        if (j.contains("smbus_timeout_flg"))
            j.at("smbus_timeout_flg").get_to(obj.smbus_timeout_flg);    
        if (j.contains("sys_in_cal"))
            j.at("sys_in_cal").get_to(obj.sys_in_cal);    
        if (j.contains("sys_in_cal_flg"))
            j.at("sys_in_cal_flg").get_to(obj.sys_in_cal_flg);    
        if (j.contains("xaxb_err"))
            j.at("xaxb_err").get_to(obj.xaxb_err);    
        if (j.contains("xaxb_err_flg"))
            j.at("xaxb_err_flg").get_to(obj.xaxb_err_flg);    
    }
    
    inline void to_json(data_t& j, const TimingSFPMonitorData& obj) {
        j["vendor_name"] = obj.vendor_name;
        j["vendor_pn"] = obj.vendor_pn;
        j["sfp_fault"] = obj.sfp_fault;
        j["ddm_supported"] = obj.ddm_supported;
        j["tx_disable_hw"] = obj.tx_disable_hw;
        j["tx_disable_sw_supported"] = obj.tx_disable_sw_supported;
        j["tx_disable_sw"] = obj.tx_disable_sw;
        j["temperature"] = obj.temperature;
        j["supply_voltage"] = obj.supply_voltage;
        j["rx_power"] = obj.rx_power;
        j["tx_power"] = obj.tx_power;
        j["laser_current"] = obj.laser_current;
        j["data_valid"] = obj.data_valid;
    }
    
    inline void from_json(const data_t& j, TimingSFPMonitorData& obj) {
        if (j.contains("vendor_name"))
            j.at("vendor_name").get_to(obj.vendor_name);    
        if (j.contains("vendor_pn"))
            j.at("vendor_pn").get_to(obj.vendor_pn);    
        if (j.contains("sfp_fault"))
            j.at("sfp_fault").get_to(obj.sfp_fault);    
        if (j.contains("ddm_supported"))
            j.at("ddm_supported").get_to(obj.ddm_supported);    
        if (j.contains("tx_disable_hw"))
            j.at("tx_disable_hw").get_to(obj.tx_disable_hw);    
        if (j.contains("tx_disable_sw_supported"))
            j.at("tx_disable_sw_supported").get_to(obj.tx_disable_sw_supported);    
        if (j.contains("tx_disable_sw"))
            j.at("tx_disable_sw").get_to(obj.tx_disable_sw);    
        if (j.contains("temperature"))
            j.at("temperature").get_to(obj.temperature);    
        if (j.contains("supply_voltage"))
            j.at("supply_voltage").get_to(obj.supply_voltage);    
        if (j.contains("rx_power"))
            j.at("rx_power").get_to(obj.rx_power);    
        if (j.contains("tx_power"))
            j.at("tx_power").get_to(obj.tx_power);    
        if (j.contains("laser_current"))
            j.at("laser_current").get_to(obj.laser_current);    
        if (j.contains("data_valid"))
            j.at("data_valid").get_to(obj.data_valid);    
    }
    
    inline void to_json(data_t& j, const TimingTLUMonitorData& obj) {
        j["cdr_lol"] = obj.cdr_lol;
        j["cdr_los"] = obj.cdr_los;
        j["mmcm_ok"] = obj.mmcm_ok;
        j["mmcm_sticky"] = obj.mmcm_sticky;
        j["pll_ok"] = obj.pll_ok;
        j["pll_sticky"] = obj.pll_sticky;
        j["sfp_flt"] = obj.sfp_flt;
        j["sfp_los"] = obj.sfp_los;
    }
    
    inline void from_json(const data_t& j, TimingTLUMonitorData& obj) {
        if (j.contains("cdr_lol"))
            j.at("cdr_lol").get_to(obj.cdr_lol);    
        if (j.contains("cdr_los"))
            j.at("cdr_los").get_to(obj.cdr_los);    
        if (j.contains("mmcm_ok"))
            j.at("mmcm_ok").get_to(obj.mmcm_ok);    
        if (j.contains("mmcm_sticky"))
            j.at("mmcm_sticky").get_to(obj.mmcm_sticky);    
        if (j.contains("pll_ok"))
            j.at("pll_ok").get_to(obj.pll_ok);    
        if (j.contains("pll_sticky"))
            j.at("pll_sticky").get_to(obj.pll_sticky);    
        if (j.contains("sfp_flt"))
            j.at("sfp_flt").get_to(obj.sfp_flt);    
        if (j.contains("sfp_los"))
            j.at("sfp_los").get_to(obj.sfp_los);    
    }
    
} // namespace dunedaq::timing::timinghardwareinfo

#endif // DUNEDAQ_TIMING_TIMINGHARDWAREINFO_NLJS_HPP