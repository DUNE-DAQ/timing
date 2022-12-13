#!/usr/bin/env python

''' 
A minimal example to set up the GIB.
For a more detailed example see `gib_debug.py`

'''

from __future__ import print_function

# Python imports
import uhal
import click
import time
from I2CuHal import I2CCore

# PDT imports
import timing.cli.toolbox as toolbox
from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.core import I2CSlave, SI534xSlave, I2CExpanderSlave, DACSlave

from click import echo, style, secho
from os.path import join, expandvars


uhal.setLogLevelTo(uhal.LogLevel.NOTICE)
verbose=False

#lConnectionsFile="${PDT_TESTS}/etc/connections.xml"
#lConnectionsFile="/home/hep/work/dune/gib/timing-dev/timing/sourcecode/timing/etc/connections.xml"
lConnectionsFile="connections.xml"
print("Connection file path=", lConnectionsFile)

lConnections  = toolbox.sanitizeConnectionPaths(lConnectionsFile)
lConnectionManager = uhal.ConnectionManager(str(lConnections))

#Replace with GIB device name
lDeviceName = "GIB_PRIMARY"

lDevice = lConnectionManager.getDevice(str(lDeviceName))

echo('Working with device ' + click.style(lDevice.id(), fg='blue'))
lDevice.dispatch()

#############################################################
# Reset Board

def sw_rst(hw):

  # Global firmware soft reset
  hw.getNode('io.csr.ctrl.soft_rst').write(0x1)
  hw.getNode('io.csr.ctrl.soft_rst').write(0x0)
  lDevice.dispatch()

#############################################################
# Reset SID to 0

def ctr_reset(hw):

  # Reset the SID counter
  hw.getNode('irig_top.csr.ctrl.clear_sid_ctr').write(0x1)
  hw.dispatch()
  hw.getNode('irig_top.csr.ctrl.clear_sid_ctr').write(0x0)
  hw.dispatch()
  print("Reset SID counter!")

#############################################################
# Enable TS Counter

def enable_ts_ctr(hw, enable):
  print("Write Enable TS:", enable)
  hw.getNode('irig_top.csr.ctrl.enable').write(enable)
  hw.dispatch()
  time.sleep(1)
  res = hw.getNode('irig_top.csr.stat.enable').read()
  hw.dispatch()
  print("Read Enable TS:", res)

#############################################################
# Force load SID counter

def force_load(hw):

  print("Force loading counter")
  hw.getNode('irig_top.csr.ctrl.soft_load').write(0x1)
  hw.getNode('irig_top.csr.ctrl.force_strobe').write(0x1)
  hw.dispatch()
                                                        
  hw.getNode('irig_top.csr.ctrl.force_strobe').write(0x0)
  hw.getNode('irig_top.csr.ctrl.soft_load').write(0x0)
  hw.dispatch()

def software_load(hw):

  print("Loading SID counter on next PPS")
  hw.getNode('irig_top.csr.ctrl.soft_load').write(0x1)
  hw.dispatch()
                                                        
  hw.getNode('irig_top.csr.ctrl.soft_load').write(0x0)
  hw.dispatch()

#############################################################
# Set timestmap

def set_timestamp(hw, ts, force=False):
  '''
  ts_source = 0 = IPBus 
  '''
  print("Set Timestamp source to IPBus=0x0")
  hw.getNode('irig_top.csr.ctrl.ts_source').write(0x0)
  hw.dispatch()

  ts_hi = ((ts >> 32) & 0xFFFFFFFF)
  ts_low = (ts & 0xFFFFFFFF)
  print("TS hi/low", ts_hi, ts_low)
  hw.getNode('irig_top.ctr.set').writeBlock([ ts_low, ts_hi ])
  hw.dispatch()

  # Need to clear SID counter which also resets the load flag
  ctr_reset(hw)

  if force:
    force_load(hw)
  else:
    software_load(hw)
  time.sleep(1)
  
  res_src = hw.getNode('irig_top.csr.stat.ts_source').read()
  res = hw.getNode('irig_top.ctr.val').readBlock(2)
  hw.dispatch()

  print("Read Timestamp Source:", res_src)
  print("Read Ctr:", res) #, " (", hex(res), ")")

#############################################################
# Set IRIG timestamp

def set_irig_timestamp(hw, force=False):

  print("Set Timestamp source to IRIG=0x1")
  hw.getNode('irig_top.csr.ctrl.ts_source').write(0x1)
  hw.dispatch()

  # Need to clear SID counter which also resets the load flag
  ctr_reset(hw)

  if force:
    force_load(hw)
  else:
    software_load(hw)

  res_src = hw.getNode('irig_top.csr.stat.ts_source').read()
  hw.dispatch()

  print("Read Timestamp Source:", res_src)

#############################################################
# Read SID

def read_sid_counter(hw):
  
  tstamp_hi = hw.getNode('irig_top.sid_hi.sid_ctr_msw').read()
  tstamp_low = hw.getNode('irig_top.sid_lo.sid_ctr_lsw').read()
  hw.dispatch()

  print("Read SID Counter:", (tstamp_hi << 32) + tstamp_low)

def read_loaded_sid(hw):
  
  tstamp_hi = hw.getNode('irig_top.load_hi.loaded_sid_msw').read()
  tstamp_low = hw.getNode('irig_top.load_lo.loaded_sid_lsw').read()
  hw.dispatch()

  print("Read Loaded SID:   ", (tstamp_hi << 32) + tstamp_low)

#############################################################
# Read Error Bit

def read_sid_error_bit(hw):
  res = hw.getNode('irig_top.csr.stat.sid_error').read()
  hw.dispatch()

  print("SID Error Bit:", res)

#############################################################
# Read Strobe Bit

def read_sid_strobe_bit(hw):
  res = hw.getNode('irig_top.csr.stat.sid_strobe').read()
  hw.dispatch()

  print("SID Strobe Bit:", res)

#############################################################
# Clear PPS Counter

def clear_pps_counter(hw):
  hw.getNode('irig_top.irig_time_source.csr.ctrl.clear_pps_ctr').write(0x1)
  hw.getNode('irig_top.irig_time_source.csr.ctrl.clear_pps_ctr').write(0x0)
  hw.dispatch()

  print("Reset PPS Counter")

#############################################################
# Read PPS Counter

def read_pps_counter(hw):
  res = hw.getNode('irig_top.irig_time_source.ctr_reg.pps_ctr').read()
  hw.dispatch()

  print("PPS Count:", res)

#############################################################
# Read IRIG time

def read_irig_time(hw):
  year = hw.getNode('irig_top.irig_time_source.csr.stat.year').read()
  day  = hw.getNode('irig_top.irig_time_source.csr.stat.day').read()
  hour = hw.getNode('irig_top.irig_time_source.csr.stat.hour').read()
  minute = hw.getNode('irig_top.irig_time_source.csr.stat.minute').read()
  second = hw.getNode('irig_top.irig_time_source.csr2.second').read()
  hw.dispatch()

  print("Year:", year)
  print("Day:", day)
  print("Hour:", hour)
  print("Minute:", minute)
  print("Second:", second)

def calc_timestamp(hw):

  year = hw.getNode('irig_top.irig_time_source.csr.stat.year').read()
  day  = hw.getNode('irig_top.irig_time_source.csr.stat.day').read()
  hour = hw.getNode('irig_top.irig_time_source.csr.stat.hour').read()
  minute = hw.getNode('irig_top.irig_time_source.csr.stat.minute').read()
  second = hw.getNode('irig_top.irig_time_source.csr2.second').read()
  hw.dispatch()

  #unix_seconds_to_2000_offset = 0xD234CCF5243000
  tai_ticks_to_1999_offset = 0x1651F5121779000
  leap_years_since_2000 = 6
  year_to_second = 0x1E13380
  day_to_second = 0x15180
  hour_to_second = 0xE10
  minute_to_second = 0x3C

  total_seconds = year * year_to_second + leap_years_since_2000 * year_to_second + day * day_to_second + hour * hour_to_second + minute * minute_to_second + second
  total_seconds *= 62.5e6
  total_seconds += tai_ticks_to_1999_offset

  print(f"Expected Timestamp: {int(total_seconds)}")
  read_loaded_sid(hw)


#########################################33

enable_ts_ctr(lDevice, 1)

read_sid_counter(lDevice)
read_loaded_sid(lDevice)

read_loaded_sid(lDevice)

set_irig_timestamp(lDevice, force=False)
read_sid_counter(lDevice)

read_sid_error_bit(lDevice)
read_sid_strobe_bit(lDevice)

read_pps_counter(lDevice)
read_irig_time(lDevice)

time.sleep(1)

calc_timestamp(lDevice)
read_sid_error_bit(lDevice)

