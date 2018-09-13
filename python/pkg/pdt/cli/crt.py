import click

import collections

import toolbox
import definitions as defs

from click import echo, style, secho
from click_texttable import Texttable
import time

# ------------------------------------------------------------------------------
#    ____        __          _      __ 
#   / __/__  ___/ /__  ___  (_)__  / /_
#  / _// _ \/ _  / _ \/ _ \/ / _ \/ __/
# /___/_//_/\_,_/ .__/\___/_/_//_/\__/ 
#              /_/                     
@click.group('crt', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device)
def crt(obj, device):
    '''
    Endpoint master commands.

    \b
    DEVICE: uhal device identifier
    IDS: id(s) of the target endpoint(s).
    '''

    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)

    echo('Created crt device')

    # Ensure that target endpoint exists

    obj.mDevice = lDevice
    obj.mEndpoint = lDevice.getNodes('endpoint0')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@crt.command('status')
@click.pass_obj
def status(obj):
    '''
    Activate timing endpoint wrapper block.
    '''

    lDevice = obj.mDevice
    lEpt = lDevice.getNode('endpoint0')
    lRegs = toolbox.readSubNodes(lEpt)
    toolbox.printRegTable(lRegs, False)

    lRegs['pulse.ctrl.en']
    lRegs['pulse.ctrl.cmd']

    lTimeLastPulse = (lRegs['pulse.ts_h']<<32)+lRegs['pulse.ts_l']
    echo( "Last Pulse Timestamp: {} ({})".format(style(str(lTimeLastPulse), fg='blue'), hex(lTimeLastPulse)) )

# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@crt.command('configure', short_help="Configure the CRT endpoint for running")
@click.argument('part', type=click.IntRange(0,4))
@click.argument('pulsecmd', type=click.Choice(defs.kCommandIDs.keys()))
@click.pass_obj
def configure(obj, part, pulsecmd):
    '''
    Activate timing endpoint wrapper block.
    '''

    lDevice = obj.mDevice
    lEpt = lDevice.getNode('endpoint0')

    lEpt.getNode('csr.ctrl.tgrp').write(part)
    lEpt.getNode('pulse.ctrl.en').write(0x1)
    lEpt.getNode('pulse.ctrl.cmd').write(defs.kCommandIDs[pulsecmd])
    lEpt.getClient().dispatch()

    lCsrStat = toolbox.readSubNodes(lEpt)
    toolbox.printRegTable(lCsrStat, False)

    pass
# ------------------------------------------------------------------------------
    