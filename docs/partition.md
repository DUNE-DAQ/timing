```
<node id="partition" description="PDTS timing partition" fwinfo="endpoint">
        <node id="csr" address="0x0" fwinfo="endpoint;width=1">
            <node id="ctrl" address="0x0">
                <node id="part_en" mask="0x1"/>
```

Enables the partition. If this is off, the partition is totally dead; no commands of any sort are sent. So (presumably) the first act of the board reader for a parition is to enable it (in fact, to disable it and then re-enable it, to ensure a clean state). Resetting the partition will of course cause all endpoints to go out of sync, but they should recover when the partition is brought back.

```
                <node id="trig_en" mask="0x2"/>
```

Enables the sending of data-generating ('trigger') commands. This bit is toggled on and off under the control of the throttling system. It should be enabled as the last act of the board reader before hitting run start (bearing in mind that 'spill start' is a data-generating command).

```
                <node id="evtctr_rst" mask="0x4"/>
```

Resets the partition event counter; the new value propagates to the endpoints immediately.

```
                <node id="trig_ctr_rst" mask="0x8"/>
```

Resets the command counters.

```
                <node id="buf_en" mask="0x10"/>
```

Enables the readout buffer of the partition. Disabling the partition also empties any remaining content in the buffer, so to 'wipe' the buffer at config time, disable it and re-enable it.

```
                <node id="run_req" mask="0x20"/>
```

Requests the start of a new run (meaning: a run start command is sent to the endpoints, and subsequently, the issuing of data-generating commands is also enabled). The run does not start immediately, since runs can only start outside a spill (see 'run_stat' below).

```
                <node id="cmd_mask" mask="0xffff0000"/>
```

Defines the set of commands that are broadcast to the endpoints in this partition.

```
            </node>
            <node id="stat" address="0x1">
                <node id="buf_err" mask="0x1"/>
```

Set if there is an overflow condition in the derandomiser buffer for the partition. This is fatal, a buffer reset is needed to recover.

```
                <node id="buf_empty" mask="0x2"/>
```

Set if the derandomiser buffer is empty.

```
                <node id="rob_warn" mask="0x4"/>
```

Set if there is an almost-full condition in the readout buffer for the partition. This would be a good time to send 'busy' to the data throttling system.

```
                <node id="rob_full" mask="0x8"/>
```

Set if the readout buffer is full. This is not fatal, but of course you have missed event data, so you probably want to stop the run.

```
                <node id="rob_empty" mask="0x10"/>
```

Set if the readout buffer is empty.

```
                <node id="run_stat" mask="0x20"/>
```

Set if the run is started.
    
So a likely cold-start procedure for the partition (not the board!) would be:

- Disable the partition, just in case
- Disable triggers
- Disable buffer
- Enable the partition
- Set the command mask
- Enable triggers

To start a run (assuming that no data has been read out between runs):

- Disable buffer
- Enable buffer
- Set run_req
- Wait for run_stat to go high

To end a run:

- Unset run_req
- Wait for run_stat to go low
- Read remaining content from buffer until data count is zero
- Disable buffer

To shut down a partition (why?):

- End the run
- Disable triggers
- Disable the parition

This means that the pre-partition-setup status needs to be:

- Board set up (clocks running and checked, etc)
- Timestamp set (if we ever do time-of-day stuff)
- Triggers from trigger enabled and gaps set
- Command generators set up
- Spills or fake spills enabled