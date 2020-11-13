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
lDeviceName = "PROD_MASTER"

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
lIO = lDevice.getNode('io')

# Global soft reset
lIO.getNode('csr.ctrl.soft_rst').write(0x1)
lIO.getNode('csr.ctrl.soft_rst').write(0x0)
lDevice.dispatch()

# Get the main I2C bus master
lI2CBusNode = lDevice.getNode("io.i2c")
    
# Do an I2C bus address scan
i2cdevices = lI2CBusNode.scan()

# Print the list of addresses which responded
print ('[{}]'.format(', '.join(hex(x) for x in i2cdevices)))

#######################################
# Now try the functions
HwRst(lDevice)
WakeI2C(lDevice)
GPSClkSetup(lDevice)

# Attempt an I2C transaction with a particular address
# True if transaction successful, False if not
#print(lI2CBusNode.ping(0x74))
#print(lI2CBusNode.ping(0x50))

#############################################################
# GPS Clock Translator Setup

def GPSClkSetup(hw):

  lGPSClk = hw.getNode('io.csr.ctrl')
  print("Set up the GPS Clock Translator")

  # Enable, active low
  lGPSClk.getNode('gps_clk_en').write(0x0)

  # Set filter to full bandwidth mode
  lGPSClk.getNode('gps_clk_fltr_a').write(0x0)
  lGPSClk.getNode('gps_clk_fltr_b').write(0x0)
  lGPSClk.dispatch()


#############################################################
# Reset Board

def HwRst(hw):

  lIORst = hw.getNode('io.csr.ctrl')
  print("Reset board")
  # Reset I2C switch and expander, active low
  lIORst.getNode('i2c_sw_rst').write(0x0)
  lIORst.getNode('i2c_exten_rst').write(0x0)
  lIORst.dispatch()

  time.sleep(0.1)

  # End reset 
  lIORst.getNode('i2c_sw_rst').write(0x1)
  lIORst.getNode('i2c_exten_rst').write(0x1)
  lIORst.dispatch()

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

def I2CExtender(lDevice):

  print("I2C Expander")
  lI2CBusNode = lDevice.getNode("io.i2c")
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

  addr = 0x48
  temp_reg = 0x0

  TempI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  # Select temp register and read it
  TempI2C.write(addr, [temp_reg], True) 
  temp = TempI2C.read(addr, 1)

  # The 
  if ( temp > 0xFF ):
    print( "Temperature: ", (0x200 - temp[0]) * ( - 0.5 ), "C" )
  else
    print( "Temperature: ", temp[0] * 0.5, "C" )
  
def TempMonitorPID(hw):

  addr = 0x48
  pid_reg = 0x7
                                               
  TempI2C = I2CCore(hw, 10, 5, "io.i2c", None)
                                               
  # Select Product ID register and read it
  TempI2C.write(addr, [pid_reg], True) 
  ID = TempI2C.read(addr, 1)
  print( "Temp Monitor ID", hex(ID) )


def TempMonitorConfig(hw):

  addr = 0x48
  config_reg = 0x1

  fault_q     = 0x0 # Number of trips before interrupt
  os_polarity = 0x0 # Interrupt active, high or low
  comp_int    = 0x0 # Comparator or interrupt mode
  shutdown    = 0x0 # Shutdown monitor
                                               
  TempI2C = I2CCore(hw, 10, 5, "io.i2c", None)
                                               
  # Select config register
  TempI2C.write(addr, [config_reg], True) 
  
  # Assemble config byte 000[Fault queue][OS Polarity][Comp/Int][Shutdown]
  config = ( fault_q << 3 ) + ( os_pol << 2 ) + (comp_int << 1 ) + ( shutdown )
  # Write the config (could also set write register and write in same step)
  TempI2C.write(addr, [config], True) 

  rconfig = TempI2C.read(addr, 1)
  print("Configuration register". rconfig)

def TempMonitorSetTrip(hw):

  addr      = 0x48                                   
  tos_reg   = 0x3
  thyst_reg = 0x2

  T_os   = 80 # [C] Temp interrupt trip
  T_hyst = 75 # [C] Temp trip hysteresis
                                               
  TempI2C = I2CCore(hw, 10, 5, "io.i2c", None)
                                               
  # Select T_os register and update trip temp
  TempI2C.write(addr, [tos_reg], True) 
  TempI2C.write(addr, [T_os], True) 
  
  # Select T_hysts register and update trip hyst temp
  TempI2C.write(addr, [thyst_reg], True) 
  TempI2C.write(addr, [T_hyst], True) 

  # Read back the T_os and T_hyst registers
  TempI2C.write(addr, [tos_reg], True) 
  rt_os = TempI2C.read(addr, 1)

  TempI2C.write(addr, [thyst_reg], True) 
  rt_hyst = TempI2C.read(addr, 1)

  print( "Temp Monitor Temp OS", rt_os * 0.5, "[C]" )
  print( "Temp Monitor Temp Hyst", rt_hyst * 0.5, "[C]" )


#############################################################
# Power Monitors

def PwrMonitor(hw):

  PwrI2C = I2CCore(hw, 10, 5, "io.i2c", None)
  lI2CBusNode = hw.getNode("io.i2c")

  pmons = {'5_0v' : 0x67, '3_3v' : 0x6A, '2_5v' : 0x69}

  # Check all monitors are online
  for m in pmons:
    print(m, " Power monitor online ", lI2CBusNode.ping(pmons[m]))

def ReadVin(hw):

  addr = 0x67
  vin_reg = [0x1E, 0x1F]
  pmons = {'5_0v' : 0x67, '3_3v' : 0x6A, '2_5v' : 0x69}
                                               
  PMonI2C = I2CCore(hw, 10, 5, "io.i2c", None)
                                               
  for m in pmons:
    # Select Vin register and read it
    PMonI2C.write(pmons[m], [vin_reg], True) 
    vin = PMonI2C.read(pmons[m], 2)
    print( "Power Monitor", m, " MSB, LSB", hex(vin[0]), ", ", hex(vin[1]) )


#############################################################
# EEPROM

def EEPROM(hw):

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

def I2CSwitch(hw, ch):
  
  SwI2C = I2CCore(hw, 10, 5, "io.i2c", None)
  # Select I2C channel 0 - 6
  SwI2C.write(0x70, [(1 << ch)], True)
  print("Selected I2C channel:", ch)

#############################################################
# Read Freq Counters

def ReadCtrs(hw, n_clk):

  ctr_map = ['Irig-b', 'GPS Clock', 'Rec Clock 0', 'Rec Clock 1']

  for i in range(n_clk): # GPS clk + 2 CDR clk + GPS input ctr = 4
    hw.getNode("io.freq.ctrl.chan_sel").write(i)
    hw.getNode("io.freq.ctrl.en_crap_mode").write(0)
    hw.dispatch()
    time.sleep(2)
    fq = hw.getNode("io.freq.freq.count").read()
    fv = hw.getNode("io.freq.freq.valid").read()
    hw.dispatch()
    print "Freq:", ctr_map[i] , int(fv), int(fq) * 1000000, " [MHz]"

