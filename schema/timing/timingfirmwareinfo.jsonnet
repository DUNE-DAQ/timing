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

    timing_fl_cmd_counter: s.record("TimingFLCmdCounter",
    [
        s.field("accepted", self.uint,
                doc="Number of commands accepted"),
        s.field("rejected", self.uint,
                doc="Number of commands rejected"),
    ],
    doc="Fixed length command counters structure"),
    
    pdi_master_fw_mon_data: s.record("PDIMasterMonitorData", 
    [
        s.field("timestamp", self.l_uint,
                doc="Timestamp"),
        s.field("spill_interface_enabled", self.bool_data, 0,
                doc="Partition spill interface enabled flag"),
    ], doc="PDI master monitor data"),

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
};

// Output a topologically sorted array.
thi + tei + moo.oschema.sort_select(timingfirmwareinfo, ns)
