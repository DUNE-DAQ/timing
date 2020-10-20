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

# Attempt an I2C transaction with a particular address
# True if transaction successful, False if not
#print(lI2CBusNode.ping(0x74))
#print(lI2CBusNode.ping(0x50))



