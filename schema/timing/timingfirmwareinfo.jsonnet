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

    timing_pdi_master_mon_data: s.record("TimingPDIMasterMonitorData", 
    [
        s.field("class_name", self.text_data, "TimingPDIMasterMonitorData",
                doc="Info class name"),
        s.field("timestamp", self.l_uint,
                doc="Timestamp"),
        s.field("spill_interface_enabled", self.bool_data, 0,
                doc="Partition spill interface enabled flag"),
        s.field("trig_interface_enabled", self.bool_data, 0,
                doc="Partition triggering enabled flag"),
        s.field("command_counters", self.timing_fl_cmd_counters_vector,
                doc="Vector of command counter structure"),
        s.field("partitions_data", self.timing_partition_mon_data_vector,
                doc="Vector of timing partition data"),
    ], 
    doc="PD-I timing master monitor data"),

    timing_pdi_master_tlu_mon_data: s.record("TimingPDIMasterTLUMonitorData", 
    [
        s.field("class_name", self.text_data, "TimingPDIMasterTLUMonitorData",
                doc="Info class name"),
        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingTLUMonitorData,
                doc="TLU hardware monitor data"),
        s.field("firmware_data", self.timing_pdi_master_mon_data,
                doc="PD-I timing master firmware monitor data"),
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),
    ], 
    doc="PD-I timing master monitor data"),

    timing_pdi_master_tlu_mon_data_debug: s.record("TimingPDIMasterTLUMonitorDataDebug", 
    [
        s.field("class_name", self.text_data, "TimingPDIMasterTLUMonitorDataDebug",
                doc="Info class name"),
        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingTLUMonitorDataDebug,
                doc="TLU hardware monitor data"),
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),
    ], 
    doc="PD-I timing master monitor data with extended hardware data"),

    timing_pdi_master_fmc_mon_data: s.record("TimingPDIMasterFMCMonitorData", 
    [
        s.field("class_name", self.text_data, "TimingPDIMasterFMCMonitorData",
                doc="Info class name"),
        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingFMCMonitorData,
                doc="FMC hardware monitor data"),
        s.field("firmware_data", self.timing_pdi_master_mon_data,
                doc="PD-I timing master firmware monitor data"),
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),
    ], 
    doc="PD-I timing master monitor data"),

    timing_pdi_master_fmc_mon_data_debug: s.record("TimingPDIMasterFMCMonitorDataDebug", 
    [
        s.field("class_name", self.text_data, "TimingPDIMasterFMCMonitorDataDebug",
                doc="Info class name"),
        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingFMCMonitorDataDebug,
                doc="FMC hardware monitor data"),
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),
    ], 
    doc="PD-I timing master monitor data with extended hardware data"),

    timing_endpoint_fmc_mon_data: s.record("TimingEndpointFMCMonitorData", 
    [
        s.field("class_name", self.text_data, "TimingEndpointFMCMonitorData",
                 doc="Info class name"),
        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingFMCMonitorData,
                doc="FMC hardware monitor data"),
        s.field("firmware_data", teih.dunedaq.timing.timingendpointinfo.TimingEndpointInfo,
                doc="Timing endpoint firmware monitor data"),
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),
    ], 
    doc="Timing endpoint monitor data"),

    timing_endpoint_fmc_mon_data_debug: s.record("TimingEndpointFMCMonitorDataDebug", 
    [
        s.field("class_name", self.text_data, "TimingEndpointFMCMonitorDataDebug",
                 doc="Info class name"),
        s.field("hardware_data", thih.dunedaq.timing.timinghardwareinfo.TimingFMCMonitorDataDebug,
                doc="FMC hardware monitor data (debug"),
        s.field("time_gathered", self.l_int, 0,
                doc="When was the data actually gathered"),
    ],
    doc="Timing endpoint monitor data"),

};

// Output a topologically sorted array.
thi + tei + moo.oschema.sort_select(timingfirmwareinfo, ns)
