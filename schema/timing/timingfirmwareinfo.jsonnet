local moo = import "moo.jsonnet";

local tei = import "timingendpointinfo.jsonnet";
local thi = import "timinghardwareinfo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.timing.timingfirmwareinfo";
local s = moo.oschema.schema(ns);

local teih = moo.oschema.hier(tei).dunedaq.timing.timingendpointinfo;
local thih = moo.oschema.hier(thi).dunedaq.timing.timinghardwareinfo;


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
    
    sent_cmd_counter: s.record("SentCommandCounter",
    [
        s.field("counts", self.uint,
                doc="Number of commands sent"),
    ],
    doc="Sent command counters structure"),
    
    spill_interface_mon_data: s.record("PDISpillInterfaceMonitorData",
    [
        s.field("spill_interface_enabled", self.bool_data,
                doc="Partition spill interface enabled flag"),
        s.field("source", self.bool_data,
                doc="configured source"),
        s.field("in_spill", self.bool_data,
                doc="In spill flag")
    ], doc="PDI spill interface node monitor data"),

    pdi_master_fw_mon_data: s.record("PDIMasterMonitorData", 
    [
        s.field("timestamp", self.l_uint,
                doc="Timestamp"),
    ], doc="master monitor data"),

    master_fw_mon_data: s.record("MasterMonitorData", 
    [
        s.field("timestamp", self.l_uint,
                doc="Timestamp"),
        s.field("ts_en", self.uint,
                doc="Timestamp tx enabled"),
        s.field("ts_err", self.uint,
                doc="Timestamp error"),
        s.field("tx_err", self.uint,
                doc="Tx error"),
        s.field("ctrs_rdy", self.uint,
                doc="Counters ready"),
    ], doc="master monitor data"),

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
    ], doc="HSI monitor data"),

    // TODO think about designs where only master/endpoint present
    timing_hw_info: s.record("TimingDeviceInfo", [
        s.field("device", self.text_data,
                doc="Device name"),
        s.field("pll_info", thih.TimingPLLMonitorData,
                doc="IO info payload"),
        s.field("master_info", self.master_fw_mon_data,
                doc="Master info payload"),
        s.field("endpoint_info", teih.TimingEndpointInfo,
                doc="Endpoint info payload")

    ], doc="Timing hw cmd structure")
};

// Output a topologically sorted array.
thi + tei + moo.oschema.sort_select(timingfirmwareinfo, ns)
