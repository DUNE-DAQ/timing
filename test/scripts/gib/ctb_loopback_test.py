#!/usr/bin/env python

''' 
Control the CTB loopback test firmware
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
# Reset

def reset_firmware(hw):

  # Firmware reset
  hw.getNode('io.csr.ctrl.rst').write(0x1)
  time.sleep(0.001)
  hw.getNode('io.csr.ctrl.rst').write(0x0)
  lDevice.dispatch()

  print("Reset firmware")

#############################################################
# Enable PRBS

def enable_prbs(hw, enable):

  # Firmware reset
  hw.getNode('io.csr.ctrl.enable_prbs').write(enable)
  lDevice.dispatch()

  print("PRBS Enable bit: ", enable)

#############################################################
# Reset Counters

def reset_counters(hw):

  # Firmware reset
  hw.getNode('io.csr.ctrl.force_ctr_rst').write(0x1)
  time.sleep(0.001)
 # hw.getNode('io.csr.ctrl.force_ctr_rst').write(0x0)
  lDevice.dispatch()

  print("Reset Bit Counters")

#############################################################
# Check Counters

def check_counters(hw, check):

  # Assert check signal
  hw.getNode('io.csr.ctrl.check_cnt').write(check)
  lDevice.dispatch()

  print("Checking Counters?", check)

#############################################################
# Read TX Counter

def read_tx_counter(hw):
  
  tx_hi = hw.getNode('io.csr.bit_ctr_tx_msb').read()
  tx_low = hw.getNode('io.csr.bit_ctr_tx_lsb.ctr_low').read()
  hw.dispatch()

  print("Read TX Counter Upper:", tx_hi & 0xFFFFFFFF)
  print("Read TX Counter Lower:", tx_low & 0xFFFFFFFF)
  print("Read TX Counter:", ((tx_hi & 0xFFFFFFFF) << 32) | (tx_low & 0xFFFFFFFF))

#############################################################
# Read RX Counter

def read_rx_counter(hw):
  
  rx_hi = hw.getNode('io.csr.bit_ctr_rx_msb').read()
  rx_low = hw.getNode('io.csr.bit_ctr_rx_lsb').read()
  hw.dispatch()

  print("Read RX Counter Upper:", rx_hi & 0xFFFFFFFF)
  print("Read RX Counter Lower:", rx_low & 0xFFFFFFFF)
  print("Read RX Counter:", ((rx_hi & 0xFFFFFFFF) << 32) | (rx_low & 0xFFFFFFFF))

######################################################

reset_firmware(lDevice)
time.sleep(0.001)

enable_prbs(lDevice, 1)
time.sleep(2)

#read_tx_counter(lDevice)
#print("+++++++++++++++++")
#read_rx_counter(lDevice)

#enable_prbs(lDevice, 0)
#reset_counters(lDevice)
time.sleep(1)
check_counters(lDevice, 1)
