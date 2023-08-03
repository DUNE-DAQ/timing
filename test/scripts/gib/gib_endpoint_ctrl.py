#!/usr/bin/env python

''' 
A minimal example to set up the endpoint0 block in the GIB.

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
# Endpoint Reset

def reset(hw):
  
  current_addr = hw.getNode('endpoint0.csr.ctrl.addr').read()
  hw.dispatch()

  enable(hw, enable=0x0, addr=0x0)
  hw.getNode('endpoint0.csr.ctrl.ctr_rst').write(0x1)
  hw.getNode('endpoint0.csr.ctrl.ctr_rst').write(0x0)
  hw.dispatch()
  enable(hw, enable=0x1, addr=current_addr)

#############################################################
# Read Timestamp

def read_timestamp(hw):

  ts = hw.getNode("endpoint0.tstamp").readBlock(2)
  hw.dispatch()
  print("Timestamp:", (ts[1] << 32) + ts[0])

#############################################################
# Enable/disable endpoint

def enable(hw, enable, addr):

  print("Endpoint enable set to:", enable)
  print("Endpoint Addr.:", addr)
  hw.getNode('endpoint0.csr.ctrl.addr').write(addr)
  hw.getNode('endpoint0.csr.ctrl.ep_en').write(enable)
  hw.dispatch()

#############################################################
# Status

def status(hw):

  ep_stat = hw.getNode('endpoint0.csr.stat.ep_stat').read()
  ep_rdy = hw.getNode('endpoint0.csr.stat.ep_rdy').read()
  ep_txen = hw.getNode('endpoint0.csr.stat.ep_txen').read()
  ctrs_rdy = hw.getNode('endpoint0.csr.stat.ctrs_rdy').read()
  hw.dispatch()

  print("++++++ ENDPOINT STATUS ++++++")
  print("ep_stat", ep_stat)
  print("ep_rdy", ep_rdy)
  print("ep_txen", ep_txen)
  print("ctrs_rdy", ctrs_rdy)
  read_timestamp(hw)


#############################################################

reset(lDevice)
enable(lDevice, 0x1, 0x1)

time.sleep(2)

status(lDevice)

