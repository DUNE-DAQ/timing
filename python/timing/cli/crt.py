import click

import collections

from . import toolbox
import timing.common.definitions as defs
from timing.common.definitions import kLibrarySupportedBoards, kLibrarySupportedDesigns

from click import echo, style, secho
import time

# ------------------------------------------------------------------------------
#    ____        __          _      __ 
#   / __/__  ___/ /__  ___  (_)__  / /_
#  / _// _ \/ _  / _ \/ _ \/ / _ \/ __/
# /___/_//_/\_,_/ .__/\___/_/_//_/\__/ 
#              /_/                     
@click.group('crt', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, shell_complete=toolbox.completeDevices)
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
    lTopDesign = lDevice.getNode('')
    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lDevice.dispatch()

    if lBoardInfo['board_type'].value() in kLibrarySupportedBoards and lBoardInfo['design_type'].value() in kLibrarySupportedDesigns:        
        lTopDesign.validate_firmware_version()
        try:
            echo(lDevice.getNode('io').get_hardware_info())
        except:
            secho("Failed to retrieve hardware information I2C issue? Initial board reset needed?", fg='yellow')
            e = sys.exc_info()[0]
            secho("Error: {}".format(e), fg='red')
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
    echo(lCRTEpt.get_status())
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
    