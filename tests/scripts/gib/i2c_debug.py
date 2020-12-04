#!/usr/bin/env python

from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import traceback
import StringIO
from I2CuHal import I2CCore
# import operator
import sys
import os

# PDT imports
import pdt
import pdt.common.definitions as defs
import pdt.cli.toolbox as toolbox
import pdt.cli.system as system
import pdt.cli.io as io
import pdt.cli.master as master
import pdt.cli.exttrig as exttrig
import pdt.cli.align as align
import pdt.cli.endpoint as endpoint
import pdt.cli.crt as crt
import pdt.cli.debug as debug

from pdt.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from pdt.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from pdt.common.definitions import kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout
from pdt.common.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap
from pdt.core import SI534xSlave, I2CExpanderSlave, DACSlave

#
from click import echo, style, secho
from os.path import join, expandvars

kLogLevelMap = {
        0: pdt.core.kError,
    }

uhal.setLogLevelTo(uhal.LogLevel.NOTICE)
verbose=False
pdt.core.setLogThreshold(kLogLevelMap.get(verbose, pdt.core.kDebug1))

lConnectionsFile="${PDT_TESTS}/etc/connections.xml"

lConnections  = toolbox.sanitizeConnectionPaths(lConnectionsFile)

lConnectionManager = uhal.ConnectionManager(str(lConnections))

#Replace with GIB device name
lDeviceName = "GIB_PRIMARY"

lDevice = lConnectionManager.getDevice(str(lDeviceName))

echo('Working with device ' + click.style(lDevice.id(), fg='blue'))

lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
lDevice.dispatch()

echo("Design '{}' on board '{}' on carrier '{}'".format(
    style(kDesignNameMap[lBoardInfo['design_type'].value()], fg='blue'),
    style(kBoardNamelMap[lBoardInfo['board_type'].value()], fg='blue'),
    style(kCarrierNamelMap[lBoardInfo['carrier_type'].value()], fg='blue')
))


# IO node
#lIO = lDevice.getNode('io')
#
## Global soft reset
#lIO.getNode('csr.ctrl.soft_rst').write(0x1)
#lIO.getNode('csr.ctrl.soft_rst').write(0x0)
#lDevice.dispatch()

#######################################
# Now try the functions
#HwRst(lDevice)
#WakeI2C(lDevice)
#GPSClkSetup(lDevice)

# Attempt an I2C transaction with a particular address
# True if transaction successful, False if not
#print(lI2CBusNode.ping(0x74))
#print(lI2CBusNode.ping(0x50))

#############################################################
# GPS Clock Translator Setup

def GPSClkSetup(hw):

  print("Set up the GPS Clock Translator")

  # Enable, active low
  hw.getNode('io.csr.ctrl.gps_clk_en').write(0x0)

  # Set filter to full bandwidth mode
  hw.getNode('io.csr.ctrl.gps_clk_fltr_a').write(0x0)
  hw.getNode('io.csr.ctrl.gps_clk_fltr_b').write(0x0)
  hw.dispatch()


#############################################################
# Reset Board

def SwRst(hw):

  # Global soft reset
  hw.getNode('io.csr.ctrl.soft_rst').write(0x1)
 # time.sleep(0.1)
  hw.getNode('io.csr.ctrl.soft_rst').write(0x0)
  lDevice.dispatch()

def HwRst(hw):

  print("Reset board")
  # Reset I2C switch and expander, active low
  hw.getNode('io.csr.ctrl.i2c_sw_rst').write(0x0)
  hw.getNode('io.csr.ctrl.i2c_exten_rst').write(0x0)
  hw.dispatch()

  time.sleep(0.1)

  # End reset 
  hw.getNode('io.csr.ctrl.i2c_sw_rst').write(0x1)
  hw.getNode('io.csr.ctrl.i2c_exten_rst').write(0x1)
  hw.dispatch()

#############################################################
# AX3 I2C Wake

def WakeI2C(hw):

  AX3I2C = I2CCore(hw, 10, 5, "io.i2c", None)
  print("Wake up I2C bus")
  
  # Wake and activate the Enclustra I2C bus
  AX3I2C.write(0x0, [8 * 0x0], True)
  AX3I2C.write(0x21, [0x01, 0x7F], True)
  

#############################################################
# I2C Expander

def I2CExtender(hw):

  # Select the correct I2C bus
  I2CSwitch(hw, 0)

  lI2CBusNode = hw.getNode("io.i2c")                       
  if( not lI2CBusNode.ping(0x74) or not lI2CBusNode.ping(0x75) ):
    print("Could not find I2C Expander at address 0x74 or 0x75")
    return

  print("I2C Expander")
  lSFPExp0 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('SFPExpander0').getI2CAddress())
  lSFPExp1 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('SFPExpander1').getI2CAddress())
  
  # Set invert registers to default for both (0,1) banks
  lSFPExp0.setInversion(0, 0x00)
  lSFPExp0.setInversion(1, 0x00)
  lSFPExp1.setInversion(0, 0x00)
  lSFPExp1.setInversion(1, 0x00)
  
  # input = 0x0, output = 0xFF
  lSFPExp0.setIO(0, 0x00)
  lSFPExp0.setIO(1, 0x00)
  lSFPExp1.setIO(0, 0x00)
  lSFPExp1.setIO(1, 0xFF)
  
  # Set SFP disable 
  lSFPExp1.setOutputs(1, 0xf0)

  res0 = lSFPExp0.readInputs(0)
  res1 = lSFPExp0.readInputs(1)
  res2 = lSFPExp1.readInputs(0)

  print ("SFP Expander 0, Bank 0", res0)
  print ("SFP Expander 0, Bank 1", res1)
  print ("SFP Expander 1, Bank 0", res2)

#############################################################
# Temp Monitor

def TempMonitor(hw):

  # Select the correct I2C bus
  I2CSwitch(hw, 0)

  lI2CBusNode = hw.getNode("io.i2c")
  if( not lI2CBusNode.ping(0x48) ):
    print("Could not find Temp. Monitor at address 0x48")
    return

  TempI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  # Select temp register and read it
  TempI2C.write(0x48, [0x0], True) 
  temp = TempI2C.read(0x48, 2)

  # Add config and trip point regs

  print("Temperature ", temp)
  
#############################################################
# Power Monitors

def PwrMonitor(hw):

  # Select the correct I2C bus
  I2CSwitch(hw, 0)

  pmons = {'5_0v' : 0xCE, '3_3v' : 0xD0, '2_5v' : 0xD2, '1_8v' : 0xD4}

  lI2CBusNode = hw.getNode("io.i2c")
  for m in pmons:
    if( not lI2CBusNode.ping( pmons[m]) ):
      print("Could not find Power Monitor at address", pmons[m])
      return

  PwrI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  # Check all monitors are online
  for m in pmons:
    print(m, " Power monitor online ", lI2CBusNode.ping(pmons[m]))

#############################################################
# EEPROM

def EEPROM(hw):

  # Select the correct I2C bus
  I2CSwitch(hw, 0)

  lI2CBusNode = hw.getNode("io.i2c")
  if( not lI2CBusNode.ping(0x50) ):
    print("Could not find EEPROM at address 0x50")
    return

  EEPROMI2C = I2CCore(hw, 10, 5, "io.i2c", None)

#############################################################
# CDR

def CDR(hw):

  CDRI2C = I2CCore(hw, 10, 5, "io.i2c", None)
  lI2CBusNode = hw.getNode("io.i2c")
  
  for cdr in range(1, 6):
    # Select the I2C channel, CDR on channels 1 - 6
    I2CSwitch(hw, cdr)
    # All CDRs have same addr but on a different I2C channelS
    print(" CDR ", cdr, " online ", lI2CBusNode.ping(0x40))

#############################################################
# I2C Switch

def I2CSwitch(hw, ch, off=False):
  
  channel = 1 << ch
  channel &= 0x7F

  if (off): channel = 0x0

  SwI2C = I2CCore(hw, 10, 5, "io.i2c", None)
  # Select I2C channel 0 - 6
  SwI2C.write(0x70, [channel], True)
  print( "Selected I2C channel:", ch, "with mask", hex(1 << ch) )
  time.sleep(0.1)
  print("Read switch register", hex( SwI2C.read(0x70,1)[0] ) )

#############################################################
# Scan all I2C Busses

def ScanAll(hw):

  lI2CBusNode = hw.getNode("io.i2c")

  # Scan all I2C switch channels
  for ch in range(0, 7):
    I2CSwitch(lDevice, ch)
    time.sleep(0.1)
    i2cdevices = lI2CBusNode.scan()

    # Print the list of addresses which responded
    print('Scanning channel', ch)
    print ('[{}]'.format(', '.join(hex(x) for x in i2cdevices)))

#############################################################
# Now try the functions

SwRst(lDevice)
WakeI2C(lDevice)
HwRst(lDevice)
GPSClkSetup(lDevice)
ScanAll(lDevice)

#I2CSwitch(lDevice, 6)
#lI2CBusNode = lDevice.getNode("io.i2c")
#print( "I2C component at 0x40", lI2CBusNode.ping(0x40) )
#I2C = I2CCore(lDevice, 10, 5, "io.i2c", None)
#for i in range(1,7):
#I2CSwitch(lDevice, 5)
 # time.sleep(0.1)
  #I2C.write(0x40, [0x0], True)
#print("Read CDR register", I2C.read(0x40,1) )
  #print( "I2C component at 0x40", lI2CBusNode.ping(0x40) )
  #print( "I2C component at 0x60", lI2CBusNode.ping(0x60) )

#vin_msb = I2C.read(0x69, 1)
#I2C.write(0x69, [0x15], True)
#vin_lsb = I2C.read(0x69, 1)
##print(vin)
#print(vin_msb," ", vin_lsb )
##print("Read 0x40 register", I2C.read(0x40,1) )
