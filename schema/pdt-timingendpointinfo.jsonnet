local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.pdt.timingendpointinfo";
local s = moo.oschema.schema(ns);

// A temporary schema construction context.
local timingendpointinfo = {
    
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

    timing_endpoint_mon_data: s.record("TimingEndpointInfo", 
    [
        s.field("state", self.uint,
                doc="Endpoint state"),
        s.field("ready", self.bool_data, 0,
                doc="Endpoint ready flag"),
        s.field("partition", self.uint, 0,
                doc="Endpoint partition"),
        s.field("address", self.uint, 0,
                doc="Endpoint address"),
        s.field("timestamp", self.l_uint, 0,
                doc="Endpoint timestamp"),
        s.field("in_run", self.bool_data, 0,
                doc="Endpoint in run flag"),
        s.field("in_spill", self.bool_data, 0,
                doc="Endpoint in spill flag"),
        s.field("buffer_warning", self.bool_data, 0,
                doc="Buffer warning flag"),
        s.field("buffer_error", self.bool_data, 0,
                doc="Buffer error flag"),
        s.field("buffer_occupancy", self.uint,
                doc="Buffer occupancy"),
        s.field("event_counter", self.uint,
                doc="Event counter"),
        s.field("reset_out", self.bool_data, 0,
                doc="Endpoint out reset line"),
        s.field("sfp_tx_disable", self.bool_data, 0,
                doc="Endpoint SFP tx disable line"),
        s.field("coarse_delay", self.uint,
                doc="Configured coarse delay"),
        s.field("fine_delay", self.uint,
                doc="Configured fine delay"),
    ], 
    doc="Timing endpoint monitor data"),
};

// Output a topologically sorted array.
moo.oschema.sort_select(timingendpointinfo, ns)
