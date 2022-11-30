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

lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
lDevice.dispatch()


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

  #############
  # FIXME switch to this!
  #lIO = hw.getNode('io.i2c').get_slave('AX3_Switch')
  #lIO.write_i2c(0x01, 0x7f)
  

#############################################################
# I2C Expander

def i2c_extender(hw, sfp_disable_mask):

  # Select the correct I2C bus, bus 0
  set_i2c_channel(hw, 0)

  print("I2C Expander")

  lI2CBusNode = hw.getNode("io.i2c")
  lIOExpander0 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.get_slave('SFPExpander0').get_i2c_address())
  lIOExpander1 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.get_slave('SFPExpander1').get_i2c_address())
  
  # Set invert registers to default for both (0,1) banks
  lIOExpander0.set_inversion(0, 0x00)
  lIOExpander0.set_inversion(1, 0x00)
  lIOExpander1.set_inversion(0, 0x00)
  lIOExpander1.set_inversion(1, 0x00)
  
  # 0: pin set as output, 1: pin set as input
  lIOExpander0.set_io(0, 0xff) # set all pins of bank 0 as inputs
  lIOExpander0.set_io(1, 0xff) # set all pins of bank 1 as inputs

  lIOExpander1.set_io(0, 0xff) # set all pins of bank 0 as inputs
  lIOExpander1.set_io(1, 0x00) # set all pins of bank 1 as outputs

  # Set SFP disable 
  # Set pins 1-6 low, i.e. enable SFP 1-6 (pins 7,8 unused)
  lIOExpander1.set_outputs(1, sfp_disable_mask) 

  res0 = lIOExpander0.read_inputs(0)
  res1 = lIOExpander0.read_inputs(1)
  res2 = lIOExpander1.read_inputs(0)

  print ("SFP Expander 0, Bank 0", res0)
  print ("SFP Expander 0, Bank 1", res1)
  print ("SFP Expander 1, Bank 0", res2)


#############################################################
# I2C Switch

def set_i2c_channel(hw, ch, disable=False):

  channel = (1 << ch) & 0x7F

  if disable: channel = 0x0
  
  SwI2C = I2CCore(hw, 10, 5, "io.i2c", None)

  # Select I2C channel 0 - 6
  SwI2C.write(0x70, [channel], True)

  print("Selected I2C channel:", ch)
  time.sleep(0.1)
  print( "I2C switch mask", hex( SwI2C.read(0x70,1)[0] ) )


#############################################################
# Configure PLL

def config_pll(hw):
  
  # Make sure correct I2C channel, ch 0
  set_i2c_channel(hw, 0)
 
  # create instance of pll class
  lI2CBusNode = hw.getNode("io.i2c")
  print("CLKGen Addr:", hex(lI2CBusNode.get_slave('ClkGen').get_i2c_address()))
  lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.get_slave('ClkGen').get_i2c_address())
  
  # read pll version
  lSIVersion = lSIChip.read_device_version()
  echo("PLL version : "+style(hex(lSIVersion), fg='blue'))
  
  # upload a pll config file
  lClockConfigPath = "SI5395Cfg/GIBDBG03-Registers.txt" # 312.5MHz all output enabled
  
  lSIChip.configure(lClockConfigPath)
  echo("PLL configuration id: {}".format(style(lSIChip.read_config_id(), fg='green')))


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
# Read PLL status

def pll_status(hw):

  pll_stat = hw.getNode('io.csr.stat.mmcm_ok').read()
  clkgen_stat = hw.getNode('io.csr.stat.clk_gen_lol').read()
  pll_sticky = hw.getNode('io.csr.stat.mmcm_sticky').read()
  lDevice.dispatch()

  print("PLL Status:", pll_stat)
  print("PLL Sticky Status:", pll_sticky)
  print("SI5395 LoL:", clkgen_stat)

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
  i.  Read frequency counters

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

# Enable all SFPs (all enabled mask = 0xC0)
i2c_extender(lDevice, sfp_disable_mask=0xC0) 

# Set the edge on which the recovered data is captured by the recovered clock
cdr_edge_select( lDevice, 1 ) # 0=rise, 1=fall

# Read the frequency counters for: GPS clock, recovered clocks, PPS, IRIG-B, SYNC
read_freq_ctrs(lDevice)

pll_status(lDevice)


