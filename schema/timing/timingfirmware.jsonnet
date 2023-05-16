local moo = import "moo.jsonnet";

// A schema builder in the given path (namespace)
local ns = "dunedaq.timing.timingfirmware";
local s = moo.oschema.schema(ns);

// A temporary schema construction context.
local timingfirmware = {
    
    bool_data: s.boolean("BoolData", doc="A bool"),

    text_data: s.string("TextData", moo.re.ident_only, 
        doc="A string field"),

    uint: s.number("RegValue", "u4", 
        doc="32 bit uint"),

    int: s.number("IntData", "i4", 
        doc="integer"),

    l_uint: s.number("LongUint", "u8",
        doc="64 bit uint"),

    l_int: s.number("LongInt", "i8",
        doc="64 bit uint"),

    double_val: s.number("DoubleValue", "f8", 
        doc="A double"),

    timing_endpoint_check_result_data: s.record("EndpointCheckResult", 
    [
        s.field("address", self.uint,
                doc="Address of the checked endpoint"),
        s.field("alive", self.bool_data, false,
                doc="Was the endpoint alive?"),
        s.field("round_trip_time", self.int, -1,
                doc="Measured endpoint round trip time"),
        s.field("state", self.int, -1,
                doc="State of the checked endpoint"),
        s.field("round_trip_time_after_delay_apply", self.int, -1,
                doc="Measured endpoint round trip time after delay apply"),
        s.field("state_after_delay_apply", self.int, -1,
                doc="State of the checked endpoint after delays applied"),
        s.field("applied_delay", self.int, -1,
                doc="Applied delay"),
    ], doc="Endpoint check result data"),

    timing_endpoint_scan_results: s.sequence("EndpointCheckResultoVector", self.timing_endpoint_check_result_data,
            doc="A vector timing endpoint check result data"),
};

// Output a topologically sorted array.
moo.oschema.sort_select(timingfirmware, ns)
