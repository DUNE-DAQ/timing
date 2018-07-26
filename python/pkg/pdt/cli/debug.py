from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import pdt

import pdt.cli.toolbox as toolbox
import pdt.cli.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars
from pdt.core import SI5344Slave, SI534xSlave, I2CExpanderSlave

kBoardSim = 0x1
kBoardFMC = 0x0
kBoardPC059 = 0x2
kBoardMicrozed = 0x3
kBoardTLU = 0x4

kCarrierEnclustraA35 = 0x0
kCarrierKC705 = 0x1
kCarrierMicrozed = 0x2

kBoardNamelMap = {
    kBoardSim: 'sim',
    kBoardFMC: 'fmc',
    kBoardPC059: 'pc059',
    kBoardMicrozed: 'microzed',
    kBoardTLU: 'tlu'
}

kCarrierNamelMap = {
    kCarrierEnclustraA35: 'enclustra-a35',
    kCarrierKC705: 'kc705',
    kCarrierMicrozed: 'microzed',
}


# ------------------------------------------------------------------------------
@click.group('debug', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device)
def debug(obj, device):
    '''
    Timing master commands.

    DEVICE: uhal device identifier
    '''
    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)
        
    echo('Created device ' + click.style(lDevice.id(), fg='blue'))

    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lDevice.dispatch()

    # print({ k:v.value() for k,v in lBoardInfo.iteritems()})
    # raise SystemExit(0)

    echo("| Board  '{}' on '{}'".format(
        style(kBoardNamelMap[lBoardInfo['board_type'].value()], fg='blue'),
        style(kCarrierNamelMap[lBoardInfo['carrier_type'].value()], fg='blue')
    ))
    echo("| Design {}".format(hex(lBoardInfo['design_type'])))

    obj.mDevice = lDevice
    obj.mBoardType = lBoardInfo['board_type'].value()
    obj.mCarrierType = lBoardInfo['carrier_type'].value()
    obj.mDesignType = lBoardInfo['design_type'].value()
# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
@debug.command('sfpexpander', short_help="Debug.")
@click.pass_obj
def sfpexpander(obj):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType

    if lBoardType != kBoardPC059:
        secho('No SFP expander on {}'.format(kBoardNamelMap[lBoardInfo['board_type'].value()]))
        return
    lI2CBusNode = lDevice.getNode("io.i2c")
    lSFPExp = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('SFPExpander').getI2CAddress())
    lSFPExpStatus = lSFPExp.debug()

    lLabels = [
        'B0 values', 
        'B1 values',
        'B0 enable',
        'B1 enable',
        'B0 invert',
        'B1 invert',
        'B0 I/O   ',
        'B1 I/O   ',
        ]
    for a,v in enumerate(lSFPExpStatus):
        echo("{} ({}): {}".format(lLabels[a], hex(a), hex(v)))
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@debug.command()
@click.pass_obj
def si5345(obj):
    lDevice = obj.mDevice

    lI2CBusNode = lDevice.getNode('io.i2c')
    lSI5345 = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())
    lClockConfigPath = 'SI5345/PDTS0005.txt'
    lFullClockConfigPath = expandvars(join('${PDT_TESTS}/etc/clock', lClockConfigPath))

    # toolbox.hookDebugger()
    lSI5345.configure(lFullClockConfigPath)
# ------------------------------------------------------------------------------
