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

#############################################################
# GPS Clock Translator Setup

def gps_clk_setup(hw):

  print("Set up the GPS Clock Translator")

  # Enable, active low
  hw.getNode('io.csr.ctrl.gps_clk_en').write(0x0)

  # Set filter to full bandwidth mode A = B = 0x0
  hw.getNode('io.csr.ctrl.gps_clk_fltr_a').write(0x0)
  hw.getNode('io.csr.ctrl.gps_clk_fltr_b').write(0x0)
  hw.dispatch()


#############################################################
# Reset Board

def sw_rst(hw):

  # Global firmware soft reset
  hw.getNode('io.csr.ctrl.soft_rst').write(0x1)
  hw.getNode('io.csr.ctrl.soft_rst').write(0x0)
  lDevice.dispatch()

def hw_rst(hw):

  print("Reset board")
  # Reset I2C switch and expander, active low
  hw.getNode('io.csr.ctrl.i2c_sw_rst').write(0x0)
  hw.getNode('io.csr.ctrl.i2c_exten_rst').write(0x0)
  hw.getNode('io.csr.ctrl.clk_gen_rst').write(0x0)
  hw.dispatch()

  time.sleep(0.1)

  # End reset 
  hw.getNode('io.csr.ctrl.i2c_sw_rst').write(0x1)
  hw.getNode('io.csr.ctrl.i2c_exten_rst').write(0x1)
  hw.getNode('io.csr.ctrl.clk_gen_rst').write(0x1)
  hw.dispatch()


#############################################################
# AX3 I2C Wake

def wake_ax3_i2c(hw):

  AX3I2C = I2CCore(hw, 10, 5, "io.i2c", None)
  print("Wake up I2C bus")
  
  # Wake and activate the Enclustra I2C bus
  AX3I2C.write(0x0, [8 * 0x0], True)
  AX3I2C.write(0x21, [0x01, 0x7F], True)
  

#############################################################
# I2C Expander

def i2c_extender(hw, sfp_disable_mask=0xC0):

  # Select the correct I2C bus, bus 0
  set_i2c_channel(hw, 0)

  print("I2C Expander")

  lI2CBusNode = hw.getNode("io.i2c")
  lIOExpander0 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('SFPExpander0').getI2CAddress())
  lIOExpander1 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('SFPExpander1').getI2CAddress())
  
  # Set invert registers to default for both (0,1) banks
  lIOExpander0.setInversion(0, 0x00)
  lIOExpander0.setInversion(1, 0x00)
  lIOExpander1.setInversion(0, 0x00)
  lIOExpander1.setInversion(1, 0x00)
  
  # 0: pin set as output, 1: pin set as input
  lIOExpander0.setIO(0, 0xff) # set all pins of bank 0 as inputs
  lIOExpander0.setIO(1, 0xff) # set all pins of bank 1 as inputs

  lIOExpander1.setIO(0, 0xff) # set all pins of bank 0 as inputs
  lIOExpander1.setIO(1, 0x00) # set all pins of bank 1 as outputs

  # Set SFP disable 
  # Set pins 1-6 low, i.e. enable SFP 1-6 (pins 7,8 unused)
  lIOExpander1.setOutputs(1, sfp_disable_mask) 

  res0 = lIOExpander0.readInputs(0)
  res1 = lIOExpander0.readInputs(1)
  res2 = lIOExpander1.readInputs(0)

  print ("SFP Expander 0, Bank 0", res0)
  print ("SFP Expander 0, Bank 1", res1)
  print ("SFP Expander 1, Bank 0", res2)


#############################################################
# Temp Monitor

def read_temp_monitor(hw):

  # Select the correct I2C bus
  set_i2c_channel(hw, 0)

  lI2CBusNode = hw.getNode("io.i2c")
  if( not lI2CBusNode.ping(0x48) ):
    print("Could not find Temp. Monitor at address 0x48")
    return

  TempI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  # Select temp register and read it
  TempI2C.write(0x48, [0x0], True) 
  temp = TempI2C.read(0x48, 2) # returns [MSB, ..., LSB]

  #print("Temperature byte 1 ", temp[1], " byte 0 ", temp[0])
  
  # Temp    D15 | D14 | D13 | D12 | D11 | D10 | D9 | D8 | D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0
  # format  MSB | b7  |  b6 |  b5 |  b4 |  b3 | b2 | b1 | LSB| X  | X  | X  | X  | X  | X | X 

  # Assuming TempI2C.read(0x48, 2) returns a list of bytes ordered as [MSB, ..., LSB] 
  t_lsb = (temp[1] >> 7) & 0x1 # temp = [MSByte,LSByte]
  t_msB = temp[0] & 0xFF       # temp = [MSByte,LSByte]

  temp_res = ( t_msB << 1 ) + t_lsb

  print("Temperature = ", temp_res * 0.5) # 1b = 0.5C


def config_temp_alerts(hw, t_thresh, t_hyst):

  ''' Set the temp trip point `t_thresh` and hysteresis `t_hyst` in degrees Celsius  '''

  # Select the correct I2C bus
  set_i2c_channel(hw, 0)

  TempI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  Thyst_reg = 0x2
  Tthresh_reg = 0x3

  # Format (2B): MSB | Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | LSB | X | X | X | X | X | X | X
  Thyst_lsb = (t_hyst & 0x1) << 7
  Thyst_msb = (t_hyst >> 1) & 0xFF

  Tthresh_lsb = (t_thresh & 0x1) << 7
  Tthresh_msb = (t_thresh >> 1) & 0xFF

  # Set temp threshold
  TempI2C.write(0x48, [Tthresh_reg, Tthresh_msb, Tthresh_lsb], True)

  TempI2C.write(0x48, [Thyst_reg, Thyst_msb, Thyst_lsb], True)

  
#############################################################
# Power Monitors

def check_pwr_monitor(hw):

  # Select the correct I2C bus
  set_i2c_channel(hw, 0)

  pmons = {'5_0v' : 0x67, '3_3v' : 0x6A, '2_5v' : 0x69}

  PwrI2C = I2CCore(hw, 10, 5, "io.i2c", None)
  lI2CBusNode = hw.getNode("io.i2c")

  for m in pmons:
    if( not lI2CBusNode.ping( pmons[m]) ):
      print("Could not find",m ," Power Monitor at address", hex(pmons[m]))
      return

  # Check all monitors are online
  for m in pmons:
    print(m, " Power monitor online ", lI2CBusNode.ping(pmons[m]))


def set_pwr_monitor_thresh(hw, thresh, addr, msb_reg, resolution):

  # Select the correct I2C bus
  set_i2c_channel(hw, 0)

  word = int(thresh / resolution) 

  LSB = (word & 0xF) << 4 # format: b7 b6 b5 b4 X X X X
  MSB = (word >> 4) & 0xFF

  PwrI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  PwrI2C.write(addr, [msb_reg], True)
  PwrI2C.write(addr, [MSB], True)
  PwrI2C.write(addr, [msb_reg + 0x1], True)
  PwrI2C.write(addr, [LSB], True)

  print("Wrote threshold: MSB = ", MSB, " LSB = ", LSB)


def config_pwr_monitor_alerts(hw):

  ''' See ReadPwrMonitor() for resolution definitions. '''

  pmons = {'5_0v' : 0x67, '3_3v' : 0x6A, '2_5v' : 0x69}
  alert_byte = 0x3C # enable I and V alerts

  # Select the correct I2C bus
  set_i2c_channel(hw, 0)

  PwrI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  # Set Alert mode for each monitor
  for m in pmons:
    PwrI2C.write(pmons[m], [0x1], True)
    PwrI2C.write(pmons[m], [alert_byte], True)

  # Set alert thresholds TODO: should be configuration

  # 5.0V config
  set_pwr_monitor_thresh(hw, thresh=5.5, addr=0x67, msb_reg=0x24, resolution=25.e-3) # Max V [V]
  set_pwr_monitor_thresh(hw, thresh=4.5, addr=0x67, msb_reg=0x26, resolution=25.e-3) # Min V [V]
  set_pwr_monitor_thresh(hw, thresh=2.0, addr=0x67, msb_reg=0x1A, resolution=5.e-3) # Max I [A]
  set_pwr_monitor_thresh(hw, thresh=0.1, addr=0x67, msb_reg=0x1C, resolution=5.e-3) # Min I [A]

  # 3.3V config
  set_pwr_monitor_thresh(hw, thresh=3.8, addr=0x6A, msb_reg=0x24, resolution=25.e-3) # Max V [V]
  set_pwr_monitor_thresh(hw, thresh=2.8, addr=0x6A, msb_reg=0x26, resolution=25.e-3) # Min V [V]
  set_pwr_monitor_thresh(hw, thresh=6.0, addr=0x6A, msb_reg=0x1A, resolution=1.25e-3) # Max I [A]
  set_pwr_monitor_thresh(hw, thresh=0.1, addr=0x6A, msb_reg=0x1C, resolution=1.25e-3) # Min I [A]

  # 2.5V config
  set_pwr_monitor_thresh(hw, thresh=3.0, addr=0x69, msb_reg=0x24, resolution=25e-3) # Max V [V]
  set_pwr_monitor_thresh(hw, thresh=2.0, addr=0x69, msb_reg=0x26, resolution=25e-3) # Min V [V]
  set_pwr_monitor_thresh(hw, thresh=5.0, addr=0x69, msb_reg=0x1A, resolution=1.25e-3) # Max I [A]
  set_pwr_monitor_thresh(hw, thresh=0.1, addr=0x69, msb_reg=0x1C, resolution=1.25e-3) # Min I [A]


def read_pwr_monitor(hw):

  ''' 

  Default mode is "continous scan" which updates values at 7.5Hz 

  Note on P, V, I conversion factors, i.e, resolution
  V res: 25mV for all voltages
  I res: For 5V is 5mA and 3.3V, 2.5V is 5mA
   --> I: I = \Delta SENSE / R where R = 0.005ohm for 5V, 3.3V, 2.5V 
  P res: For 5V,  3.3V, 2.5V is 125uW
   --> P = I * V and using the respective resolutions.

  '''

  pmons = {'5_0v' : 0x67, '3_3v' : 0x6A, '2_5v' : 0x69}
  v_list = []
  i_list = []

  # Select the correct I2C bus
  set_i2c_channel(hw, 0)

  PwrI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  # Read the 3 POWER registers, 24b
  for m in pmons:
    PwrI2C.write(pmons[m], [0x5], False)
    # Subsequent reads increment register address by 0x1, regs stops incrementing when a STOP is sent
    pwr_msb2 = PwrI2C.read(pmons[m], 1)[0]
    pwr_msb1 = PwrI2C.read(pmons[m], 1)[0]
    pwr_lsb1 = PwrI2C.read(pmons[m], 1)[0]
    # print("Power ", pwr_msb2, " ", pwr_msb1, " ", pwr_lsb1)
    power = (pwr_msb2 << 16) + (pwr_msb1 << 8) + (pwr_lsb1)
    print(m, " Power = ", power * 1.25e-4, " [W]") # 1.25e-4 [W] resolution

  # Read the voltage registers 12b
  for m in pmons:
    PwrI2C.write(pmons[m], [0x1E], False)
    # Read the 2 Vin regs
    vol_msb = PwrI2C.read(pmons[m], 1)[0]
    vol_lsb = PwrI2C.read(pmons[m], 1)[0]
    voltage = (vol_msb << 4) + ((vol_lsb >> 4) & 0xF)
    v_list.append(voltage * 25e-3)
    print(m, " Voltage = ", voltage * 25e-3, " [V]") # 25mV voltage resolution

  # Read the current registers 12b
  for m in pmons:
    PwrI2C.write(pmons[m], [0x14], False)
    # Read the 2 \Delta SENSE regs
    curr_msb = PwrI2C.read(pmons[m], 1)[0]
    curr_lsb = PwrI2C.read(pmons[m], 1)[0]
    current = (curr_msb << 4) + ((curr_lsb >> 4) & 0xF)
    i_list.append(current * 5.e-3)
    print(m, " Current = ", current * 5.e-3, " [A]") # 5mA current resolution

  # Use the I, V read previously to calculate power
  for m, v, i in zip(pmons, v_list, i_list):
    print(m, " Calculated Power = ", v * i," [W]")


#############################################################
# EEPROM

def read_eeprom(hw, reg_addr, last_reg=None):

  '''  
  Read memory of 24AA025E-48T 
  EEPROM user register range 0x0 - 0x80
  '''

  reg_range = 0
  if last_reg is not None:
    reg_range = (last_reg - reg_addr) + 1

  # Select the correct I2C bus
  set_i2c_channel(hw, 0)

  lI2CBusNode = hw.getNode("io.i2c")

  if( not lI2CBusNode.ping(0x50) ):
    print("Could not find EEPROM at address 0x50")
    return

  EEPROMI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  for inc in range(0, reg_range):
    EEPROMI2C.write(0x50, [reg_addr + inc], True)
    res = EEPROMI2C.read(0x50, 1)[0]
    print("EEPROM Register ", hex(reg_addr + inc), ": ", hex(res))


def set_eeprom(hw, reg_addr, byte_list):

  '''  
  Write to memory of 24AA025E-48T 
  EEPROM user register range 0x0 - 0x80
  '''

  # Select the correct I2C bus
  set_i2c_channel(hw, 0)

  EEPROMI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  EEPROMI2C.write(0x50, [reg_addr] + byte_list, True)
  

#############################################################
# CDR

def find_cdr(hw):

  CDRI2C = I2CCore(hw, 10, 5, "io.i2c", None)
  lI2CBusNode = hw.getNode("io.i2c")
  
  for cdr in range(1, 6):

    # Select the I2C channel, CDR on channels 1 - 6
    set_i2c_channel(hw, cdr)

    # All CDRs have same addr but on a different I2C channelS
    print(" CDR ", cdr, " online ", lI2CBusNode.ping(0x40))


#############################################################
# I2C Switch

def set_i2c_channel(hw, ch, off=False):

  channel = (1 << ch) & 0x7F

  if (off): channel = 0x0
  
  SwI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  # Select I2C channel 0 - 6
  SwI2C.write(0x70, [channel], True)

  print("Selected I2C channel:", ch)
  time.sleep(0.1)
  print( "Read switch register", hex( SwI2C.read(0x70,1)[0] ) )


#############################################################
# Scan I2C Busses

def scan_single_i2c_bus(hw, ch):

  lI2CBusNode = hw.getNode("io.i2c")

  set_i2c_channel(lDevice, ch)

  i2cdevices = lI2CBusNode.scan()
  # Print the list of addresses which responded
  print ('[{}]'.format(', '.join(hex(x) for x in i2cdevices)))

def scan_all_i2c_bus(hw):

  for ch in range(0, 7):
    scan_single_i2c_bus(hw, ch)


#############################################################
# Configure PLL

def config_pll(hw):
  
  # Make sure correct I2C channel, ch 0
  set_i2c_channel(hw, 0)
 
  # create instance of pll class
  lI2CBusNode = hw.getNode("io.i2c")
  lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('ClkGen').getI2CAddress())
  
  # read pll version
  lSIVersion = lSIChip.readDeviceVersion()
  echo("PLL version : "+style(hex(lSIVersion), fg='blue'))
  
  # upload a pll config file
  lClockConfigPath = "SI5395Cfg/GIBDBG03-Registers.txt" # 312.5MHz all output enabled
  
  lSIChip.configure(lClockConfigPath)
  echo("PLL configuration id: {}".format(style(lSIChip.readConfigID(), fg='green')))


#############################################################
# Read Freq Counters

def read_freq_ctrs(hw):

  ctrs = [['GPS Clock', 1], ['Rec Clock 0', 1], ['Rec Clock 1', 1], ['Rec Clock 2', 1], ['Rec Clock 3', 1],
          ['Rec Clock 4', 1], ['Rec Clock 5', 1], ['Irig-b', 64], ['PPS', 64], ['SYNC', 64]]
  
  for ctr in ctrs:
  
    hw.getNode('io.freq.ctrl.chan_sel').write( ctrs.index(ctr) )
    hw.getNode('io.freq.ctrl.en_crap_mode').write(0x0)
    hw.dispatch()
  
    time.sleep(2)
  
    fq = hw.getNode('io.freq.freq.count').read()
    fv = hw.getNode('io.freq.freq.valid').read()
    hw.dispatch()
  
    print( "Freq:", ctr[0], int(fv), int(fq) * 119.20928 / (ctr[1] * 1e6), "[MHz]" )


#############################################################
# Select recovered clock edge

def cdr_edge_select(hw, ch):

  if ch > 1: 
    print("Edge sel out of range! Must be 0 or 1") 
    return

  hw.getNode('io.csr.ctrl.cdr_edge_sel').write( ch )
  hw.dispatch()
 
  time.sleep(1)

  r_ch = hw.getNode('io.csr.ctrl.cdr_edge_sel').read() 
  hw.dispatch()

  print( "Edge (0 = rise, 1 = fall) selected ", r_ch )


#############################################################
# Reset and configure the board
''' 

The following function calls will
1. Reset
  i.   The firmware
  ii.  The components on the GIB
2. Configure 
  i.   The GPS clock translator
  ii.  The clock generator (SI5395)
  iii. Enable all SFP transmit
  iv.  Select recovered clock capture edge
3. Status
  i.   Read temperature
  ii.  Read frequency counters

'''

# Reset firmware and hardware
sw_rst(lDevice)
hw_rst(lDevice)

# Wake the AX3 I2C
wake_ax3_i2c(lDevice)

# Setup the GPS clock translator
gps_clk_setup(lDevice)

# Configure SI5395 clock generator
config_pll(lDevice)

time.sleep(1)

# Enable all SFPs
i2c_extender(lDevice, sfp_disable_mask=0xC0) # 0xC0

# Set the edge on which the recovered data is captured by the recovered clock
cdr_edge_select( lDevice, 1 ) # 0=rise, 1=fall

# Read the temperature
read_temp_monitor(lDevice)
config_temp_alerts(lDevice, t_thresh=85, t_hyst=80)

# Read the frequency counters for: GPS clock, recovered clocks, PPS, IRIG-B, SYNC
read_freq_ctrs(lDevice)

# Read voltage and current 
read_pwr_monitor(lDevice)

# Configure power monitor alerts
#config_pwr_monitor_alerts(lDevice)

# Set EEPROM
set_eeprom(lDevice, reg_addr=0x2, byte_list=[0xA, 0x3, 0x6, 0x12, 0x15, 0x1, 0x1F])

# Read EEPROM
read_eeprom(lDevice, reg_addr=0x2, last_reg=0xA)


