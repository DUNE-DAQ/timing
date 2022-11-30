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
  hw.getNode('irig_top.csr.ctrl.force_strobe').write(0x1)
  hw.dispatch()
                                                        
  hw.getNode('irig_top.csr.ctrl.force_strobe').write(0x0)
  hw.dispatch()

#############################################################
# Set timestmap

def set_timestamp(hw, ts):
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

  force_load(hw)
  time.sleep(1)
  
  res_src = hw.getNode('irig_top.csr.stat.ts_source').read()
  res = hw.getNode('irig_top.ctr.val').readBlock(2)
  hw.dispatch()

  print("Read Timestamp Source:", res_src)
  print("Read Ctr:", res) #, " (", hex(res), ")")

#############################################################
# Set IRIG timestamp

def set_irig_timestamp(hw):
  print("Set Timestamp source to IRIG=0x1")
  hw.getNode('irig_top.csr.ctrl.ts_source').write(0x1)
  hw.getNode('irig_top.csr.ctrl.force_strobe').write(0x1)
  hw.dispatch()

  hw.getNode('irig_top.csr.ctrl.force_strobe').write(0x0)
  res_src = hw.getNode('irig_top.csr.stat.ts_source').read()
  hw.dispatch()

  print("Read Timestamp Source:", res_src)

#############################################################
# Enable Irig

def read_tstamp(hw):
  
  tstamp_hi = hw.getNode('irig_top.sr_hi.sid_msw').read()
  tstamp_low = hw.getNode('irig_top.sr_lo.sid_lsw').read()
  hw.dispatch()

  print("Read Timestamp Upper:", tstamp_hi)
  print("Read Timestamp Lower:", tstamp_low)
  print("Read Timestamp:", tstamp_hi + tstamp_low)

#############################################################
# Read Error Bit

def read_sid_error_bit(hw):
  res = hw.getNode('irig_top.csr.stat.sid_error').read()
  hw.dispatch()

  print("SID Error Bit:", res)

#########################################33

# Reset firmware and hardware
#sw_rst(lDevice)

enable_ts_ctr(lDevice, 1)
read_tstamp(lDevice)

set_timestamp(lDevice, 0)
read_tstamp(lDevice) 

set_irig_timestamp(lDevice)
read_tstamp(lDevice)

read_sid_error_bit(lDevice)

enable_ts_ctr(lDevice, 0)
