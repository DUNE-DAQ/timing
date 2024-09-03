import click
import sys

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
@click.group('irig', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, shell_complete=toolbox.completeDevices)
def irig(obj, device):
    '''
    IRIG commands.

    \b
    DEVICE: uhal device identifier
    IDS: id(s) of the target endpoint(s).
    '''

    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)

    
    echo('Created IRIG device')
    lTopDesign = lDevice.getNode('')
    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lDevice.dispatch()

    if lBoardInfo['board_type'].value() in kLibrarySupportedBoards and lBoardInfo['design_type'].value() in kLibrarySupportedDesigns:
        lTopDesign.validate_firmware_version()
        try:
            echo(lDevice.getNode('io').get_hardware_info())
        except:
            secho("Failed to retrieve hardware information! I2C issue? Initial board reset needed?", fg='yellow')
            e = sys.exc_info()[0]
            secho("Error: {}".format(e), fg='red')

    obj.mDevice = lDevice
    obj.mTopDesign = lDevice.getNode('')
    obj.mIRIG = lDevice.getNode('irig_time_source')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@irig.command('status')
@click.pass_obj
@click.pass_context
def status(ctx, obj):
    '''
    Print the status of IRIG block.
    '''

    lDevice = obj.mDevice
    lIRIG = obj.mIRIG
    
    echo(lIRIG.get_status())
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@irig.command('set-epoch', short_help="Set IRIG epoch: 0 - TAI, 1 - UNIX")
@click.pass_obj
@click.argument('epoch', type=toolbox.IntRange(0x0,0x1))
def synctime(obj, epoch):
    
    lDevice = obj.mDevice
    lIRIG = obj.mIRIG
    lIRIG.set_irig_epoch(epoch)
# ------------------------------------------------------------------------------