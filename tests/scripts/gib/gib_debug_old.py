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
import time
from I2CuHal import I2CCore

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
lIO = lDevice.getNode('io')

# Global soft reset
lIO.getNode('csr.ctrl.soft_rst').write(0x1)
lIO.getNode('csr.ctrl.soft_rst').write(0x0)
lDevice.dispatch()

# Reset board chips
print("Reset I2C switch")
lIO.getNode('csr.ctrl.i2c_sw_rst').write(0x0)
lDevice.dispatch()
time.sleep(0.5)
lIO.getNode('csr.ctrl.i2c_sw_rst').write(0x1)
lDevice.dispatch()

# Make sure GPS clock disabled and filters set
print("Set up GPS clock translator")
lIO.getNode('csr.ctrl.gps_clk_en').write(0x1)
# Low, Low gives full bandwidth
lIO.getNode('csr.ctrl.gps_clk_fltr_a').write(0x0)
lIO.getNode('csr.ctrl.gps_clk_fltr_b').write(0x0)
lDevice.dispatch()
time.sleep(1)

print ("Enable GPS Clock")
lIO.getNode('csr.ctrl.gps_clk_en').write(0x0)
lDevice.dispatch()


mmcm_ok = lIO.getNode('csr.stat.mmcm_ok').read()
temp = lIO.getNode('csr.stat.temp_intr').read()
sfp_dis = lIO.getNode('csr.stat.sfp_tx_disable').read()
lDevice.dispatch()
print("MMCM OK ", mmcm_ok)
print("Temp Interrupt ", temp)
print("SFP Tx Disable ", sfp_dis)

#Try reading EP status
# doesn't work
#lEP = lDevice.getNode('endpoint')
#print ( lEP.getNode('csr.stat.ep_stat').read() )
#lDevice.dispatch()

# Get the main I2C bus master
lI2CBusNode = lDevice.getNode("io.i2c")
    
# Do an I2C bus address scan
i2cdevices = lI2CBusNode.scan()

# Print the list of addresses which responded
print ('[{}]'.format(', '.join(hex(x) for x in i2cdevices)))

# Attempt an I2C transaction with a particular address
# True if transaction successful, False if not
print(lI2CBusNode.ping(0x21))
#print(lI2CBusNode.ping(0x50))
#

i2c = I2CCore(lDevice, 10, 5, "io.i2c", None)

#Trying something
i2c.write(0x0, [8 * 0x0], True) #Wake up AX3 EEPROM ?
time.sleep(0.5)

# Activate I2C bus on Enclustra board
i2c.write(0x21, [0x01, 0x7F], True)

# Mask in the I2C channel 
i2c.write(0x70, [0x1], True)

# Read the I2C extender 1
print("Read I2C extender 1")
res = i2c.read(0x74, 6)

for r in res:
  print( hex(r) )
#i2c.write(0x50, [0x01], True)

# Do an I2C bus address scan
i2cdevices2 = lI2CBusNode.scan()

# Print the list of addresses which responded
print ('[{}]'.format(', '.join(hex(x) for x in i2cdevices2)))
