#!/usr/bin/env python

''' 
A minimal example to set up the master block in the GIB.

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
# Reset GIB hardware and firmware

def sw_rst(hw):

  # GIB Global firmware soft reset
  hw.getNode('io.csr.ctrl.soft_rst').write(0x1)
  hw.getNode('io.csr.ctrl.soft_rst').write(0x0)
  lDevice.dispatch()

#############################################################
# Set Timestamp

def set_timestamp(hw):
  
  seconds_since_unix_epoch = int(time.time())
  print("Seconds since Unix epoch =", seconds_since_unix_epoch)

  ts = seconds_since_unix_epoch * 62500000
  ts_hi  = ((ts >> 32) & 0xFFFFFFFF)
  ts_low = (ts & 0xFFFFFFFF)
  print("TS hi/low/full", ts_hi, "/", ts_low, "/", ts)

  hw.getNode('master.tstamp.ctr.set').writeBlock([ ts_low, ts_hi ])
  hw.dispatch()

#############################################################
# Read Timestamp

def read_timestamp(hw):

  ts = hw.getNode("master.tstamp.ctr.val").readBlock(2)
  hw.dispatch()
  print("Timestamp:", (ts[1] << 32) + ts[0])

#############################################################
# Sync Master time

def synctime(hw):

  set_timestamp(hw)
  read_timestamp(hw)

  hw.getNode('master.global.csr.ctrl.ts_en').write(0x1)
  hw.dispatch()

#############################################################
# Status

def cdr_status(hw):

  locked = hw.getNode('cdr.stat.locked').read()
  los = hw.getNode('cdr.stat.los').read()
  phase = hw.getNode('cdr.stat.phase').read()
  err = hw.getNode('cdr.stat.err').read()
  valid = hw.getNode('cdr.stat.valid').read()
  hw.dispatch()

  print("++ CDR BLOCK ++")
  print("Locked: ", locked)
  print("LOS:    ", los)
  print("Phase:  ", phase)
  print("Err:    ", err)
  print("Valid:  ", hex(valid))

def status(hw):

  ts_en = hw.getNode('master.global.csr.ctrl.ts_en').read()
  tx_err = hw.getNode('master.global.csr.stat.tx_err').read()
  rx_rdy = hw.getNode('master.global.csr.stat.rx_rdy').read()
  ctrs_rdy = hw.getNode('master.global.csr.stat.ctrs_rdy').read()
  ts_err = hw.getNode('master.global.csr.stat.ts_err').read()
  cdr_locked = hw.getNode('master.global.csr.stat.cdr_locked').read()
  hw.dispatch()

  print("++++++ MASTER STATUS ++++++")
  print("ts_en:     ", ts_en)
  print("rx_rdy:    ", rx_rdy)
  print("ctrs_rdy:  ", ctrs_rdy)
  print("ts_err:    ", ts_err)
  print("cdr_locked:", cdr_locked)

  cdr_status(hw=hw)

  read_timestamp(hw)


#############################################################

synctime(lDevice)

time.sleep(2)

status(lDevice)

