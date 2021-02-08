import click

import collections

from . import toolbox
from . import definitions as defs
from pdt.common.definitions import kLibrarySupportedBoards

from click import echo, style, secho
from .click_texttable import Texttable
import time

# ------------------------------------------------------------------------------
#    ____        __          _      __ 
#   / __/__  ___/ /__  ___  (_)__  / /_
#  / _// _ \/ _  / _ \/ _ \/ / _ \/ __/
# /___/_//_/\_,_/ .__/\___/_/_//_/\__/ 
#              /_/                     
@click.group('crt', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, autocompletion=toolbox.completeDevices)
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
    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lDevice.dispatch()

    if lBoardInfo['board_type'].value() in kLibrarySupportedBoards:
        echo(lDevice.getNode('io').getHardwareInfo())
    # Ensure that target endpoint exists

    obj.mDevice = lDevice
    obj.mCRTEndpoint = lDevice.getNode('endpoint0')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@crt.command('status')
@click.pass_obj
def status(obj):
    '''
    Print the status of CRT endpoint wrapper block.
    '''

    lDevice = obj.mDevice
    lCRTEpt = obj.mCRTEndpoint
    echo(lCRTEpt.getStatus())
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
    lCRTEpt = obj.mCRTEndpoint
    lCRTEpt.enable(part,defs.kCommandIDs[pulsecmd])
    secho("CRT endpoint configured; partition: {}; cmd: {}".format(part, pulsecmd), fg='green')
# ------------------------------------------------------------------------------
    