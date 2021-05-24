local moo = import "moo.jsonnet";

local tei = import "timingendpointinfo.jsonnet";
local thi = import "timinghardwareinfo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.timing.timingfirmwareinfo";
local s = moo.oschema.schema(ns);

local teih = moo.oschema.hier(tei);
local thih = moo.oschema.hier(thi);


// A temporary schema construction context.
local timingfirmwareinfo = {
    
    bool_data: s.boolean("BoolData", doc="A bool"),

    text_data: s.string("TextData", moo.re.ident_only, 
        doc="A string field"),

    uint: s.number("RegValue", "u4", 
        doc="32 bit uint"),

    l_uint: s.number("LongUint", "u8",
        doc="64 bit uint"),

    l_int: s.number("LongInt", "i8",
        doc="64 bit uint"),

    double_val: s.number("DoubleValue", "f8", 
        doc="A double"),

    // firmware monitoring structures
    timing_partition_mon_data: s.record("TimingPartitionMonitorData", 
    [
        s.field("enabled", self.bool_data, 0,
                doc="Partition enabled flag"),
        s.field("spill_interface_enabled", self.bool_data, 0,
                doc="Partition spill interface enabled flag"),
        s.field("trig_enabled", self.bool_data, 0,
                doc="Partition triggering enabled flag"),
        s.field("trig_mask", self.uint,
                doc="Partition trigger mask"),
        s.field("rate_ctrl_enabled", self.bool_data, 0,
                doc="Partition rate control enabled flag"),
        s.field("frag_mask", self.uint,
                doc="Partition fragment mask"),
        s.field("in_run", self.bool_data, 0,
                doc="Partition in run flag"),
        s.field("in_spill", self.bool_data, 0,
                doc="Partition in spill flag"),
        s.field("buffer_enabled", self.bool_data, 0,
                doc="Buffer enabled flag"),
        s.field("buffer_warning", self.bool_data, 0,
                doc="Buffer warning flag"),
        s.field("buffer_error", self.bool_data, 0,
                doc="Buffer error flag"),
        s.field("buffer_occupancy", self.uint,
                doc="Buffer occupancy"),
        s.field("command_counters", self.timing_fl_cmd_counters_vector,
                doc="Vector of command counter structure"),
    ], 
    doc="Timing partition monitor data"),

    timing_partition_mon_data_vector: s.sequence("TimingPartitionMonitorDataVector", self.timing_partition_mon_data,
            doc="A vector timing partition data"),

    timing_fl_cmd_counters: s.record("TimingFLCmdCounters",
    [
        s.field("accepted", self.uint,
                doc="Number of commands accepted"),
        s.field("rejected", self.uint,
                doc="Number of commands rejected"),
    ],
    doc="Fixed length command counters structure"),

    timing_fl_cmd_counters_vector: s.sequence("TimingFLCmdCountersVector", self.timing_fl_cmd_counters,
                doc="A vector command counters object"),

    pdi_master_fw_mon_data: s.record("PDIMasterMonitorData", 
    [
        s.field("timestamp", self.l_uint,
                doc="Timestamp"),
        s.field("command_counters", self.timing_fl_cmd_counters_vector,
                doc="Vector of command counter structure"),
        s.field("partitions_data", self.timing_partition_mon_data_vector,
                doc="Vector of timing partition data"),
        s.field("spill_interface_enabled", self.bool_data, 0,
                doc="Partition spill interface enabled flag"),
    ], doc="PDI master monitor data"),


    overlord_tlu_mon_data: s.record("OverlordTLUMonitorData", 
    [
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),

        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingTLUMonitorData,
                doc="TLU hardware monitor data"),
        s.field("master_data", self.pdi_master_fw_mon_data,
                doc="PD-I timing master firmware monitor data"),
        
        s.field("trig_interface_enabled", self.bool_data, 0,
                doc="External triggering enabled flag"),
        
    ], 
    doc="Overlord on TLU monitor data"),

    overlord_tlu_mon_data_debug: s.record("OverlordTLUMonitorDataDebug", 
    [
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),

        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingTLUMonitorDataDebug,
                doc="TLU hardware monitor data"),
        s.field("master_data", self.pdi_master_fw_mon_data,
                doc="PD-I timing master firmware monitor data"),
        
        s.field("trig_interface_enabled", self.bool_data, 0,
                doc="External triggering enabled flag"),
    ], 
    doc="Overlord on TLU monitor data with extended hardware data"),

    overlord_fmc_mon_data: s.record("OverlordFMCMonitorData", 
    [
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),

        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingFMCMonitorData,
                doc="FMC hardware monitor data"),
        s.field("master_data", self.pdi_master_fw_mon_data,
                doc="PD-I timing master firmware monitor data"),

        s.field("trig_interface_enabled", self.bool_data, 0,
                doc="External triggering enabled flag"),
    ], 
    doc="Overlord on FMC monitor data"),

    overlord_fmc_mon_data_debug: s.record("OverlordFMCMonitorDataDebug", 
    [
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),

        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingFMCMonitorDataDebug,
                doc="FMC hardware monitor data"),
        s.field("master_data", self.pdi_master_fw_mon_data,
                doc="PD-I timing master firmware monitor data"),
        
        s.field("trig_interface_enabled", self.bool_data, 0,
                doc="External triggering enabled flag"),
    ], 
    doc="Overlord on FMC monitor data with extended hardware data"),


    // Boreas designs

    hsi_fw_mon_data: s.record("HSIFirmwareMonitorData", 
    [
        s.field("source", self.uint,
                doc="Source of HSI data"),
        
        s.field("re_mask", self.uint,
                doc="Rising edge mask"),
        
        s.field("fe_mask", self.uint,
                doc="Falling edge mask"),
        
        s.field("inv_mask", self.uint,
                doc="Inverted mask"),
        
        s.field("buffer_enabled", self.bool_data,
                doc="Buffer enable flag"),

        s.field("buffer_error", self.bool_data,
                doc="Buffer enable flag"),

        s.field("buffer_warning", self.bool_data,
                doc="Buffer enable flag"),

        s.field("buffer_occupancy", self.uint,
                doc="Number of words in buffer"),
        
        s.field("enabled", self.bool_data,
                doc="HSI triggering enabled"),
        
        s.field("endpoint_enabled", self.bool_data,
                doc="HSI endpoint enabled"),
        
        s.field("endpoint_address", self.uint,
                doc="HSI endpoint address"),
        
        s.field("endpoint_partition", self.uint,
                doc="HSI endpoint partition"),
        
        s.field("endpoint_state", self.uint,
                doc="HSI endpoint state"),
    ], doc="HSI monitor data"),

    boreas_tlu_mon_data: s.record("BoreasTLUMonitorData", 
    [
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),

        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingTLUMonitorData,
                doc="TLU hardware monitor data"),
        s.field("master_data", self.pdi_master_fw_mon_data,
                doc="PDI master monitor data"),
        
        s.field("hsi_data", self.hsi_fw_mon_data,
                doc="HSI monitor data"),
        
    ], doc="Boreas on TLU monitor data"),

    boreas_tlu_mon_data_debug: s.record("BoreasTLUMonitorDataDebug", 
    [
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),

        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingTLUMonitorDataDebug,
                doc="TLU hardware monitor data"),
        s.field("master_data", self.pdi_master_fw_mon_data,
                doc="PDI master monitor data"),
        
        s.field("hsi_data", self.hsi_fw_mon_data,
                doc="HSI monitor data"),
        
    ], doc="Boreas on TLU monitor data with extended hardware data"),

    boreas_fmc_mon_data: s.record("BoreasFMCMonitorData", 
    [
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),

        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingFMCMonitorData,
                doc="FMC hardware monitor data"),
        s.field("master_data", self.pdi_master_fw_mon_data,
                doc="PDI master monitor data"),
        
        s.field("hsi_data", self.hsi_fw_mon_data,
                doc="HSI monitor data"),
    ], doc="Boreas on FMC monitor data"),

    boreas_fmc_mon_data_debug: s.record("BoreasFMCMonitorDataDebug", 
    [
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),

        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingFMCMonitorDataDebug,
                doc="FMC hardware monitor data"),
        s.field("master_data", self.pdi_master_fw_mon_data,
                doc="PDI master monitor data"),
        
        s.field("hsi_data", self.hsi_fw_mon_data,
                doc="HSI monitor data"),
    ], doc="Boreas on FMC monitor data with extended hardware data"),


    // Endpoint design
    timing_endpoint_fmc_mon_data: s.record("TimingEndpointFMCMonitorData", 
    [
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),

        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingFMCMonitorData,
                doc="FMC hardware monitor data"),
        s.field("endpoint_data", teih.dunedaq.timing.timingendpointinfo.TimingEndpointInfo,
                doc="Timing endpoint firmware monitor data"),
    ], doc="Timing endpoint monitor data"),

    timing_endpoint_fmc_mon_data_debug: s.record("TimingEndpointFMCMonitorDataDebug", 
    [
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),

        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingFMCMonitorDataDebug,
                doc="FMC hardware monitor data (debug"),
        s.field("endpoint_data", teih.dunedaq.timing.timingendpointinfo.TimingEndpointInfo,
                doc="Timing endpoint firmware monitor data"),
    ], doc="Timing endpoint monitor data"),

    
    timing_device_data: s.any("TimingDeviceData", 
                    doc="Generic structure for timing hw device data"),

    timing_device_data_vector: s.sequence("TimingDeviceDataVector", self.timing_device_data,
            doc="A vector of timing device data"),

    timing_devices_data: s.record("TimingDevicesData",
    [
        s.field("collector", self.text_data, "Collector",
                doc="Name of module collecting the data"),
        s.field("device_data", self.timing_device_data_vector,
                doc="Sequence of timing device data")
    ],
    doc="Contained to hold all gathered timing device data"),
};

// Output a topologically sorted array.
thi + tei + moo.oschema.sort_select(timingfirmwareinfo, ns)
