import click
import sys

import collections

from . import toolbox
import timing.common.definitions as defs
from timing.common.definitions import kLibrarySupportedBoards, kLibrarySupportedDesigns, TimestampEpoch, TimestampTimebase

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
@irig.command('set-epoch', short_help="Set ts epoch: UNIX or custom")
@click.pass_obj
@click.argument('epoch',  type=click.Choice(TimestampEpoch.__members__.keys()))
def set_epoch(obj, epoch):

    lDevice = obj.mDevice
    lIRIG = obj.mIRIG

    lEpoch=TimestampEpoch.__members__[epoch]
    lIRIG.set_ts_epoch(lEpoch)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@irig.command('set-epoch-value', short_help="Set ts epoch (custom mode)")
@click.pass_obj
@click.argument('epoch_to_2000_seconds_tai', type=int)
@click.argument('epoch_to_2000_leap_seconds', type=int)
def set_epoch_value(obj, epoch_to_2000_seconds_tai, epoch_to_2000_leap_seconds):

    lDevice = obj.mDevice
    lIRIG = obj.mIRIG

    lIRIG.set_ts_epoch_value(epoch_to_2000_seconds_tai, epoch_to_2000_leap_seconds)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@irig.command('set-timebase', short_help="Set ts timebase: TAI or UTC")
@click.pass_obj
@click.argument('timebase',  type=click.Choice(TimestampTimebase.__members__.keys()))
def set_timebase(obj, timebase):

    lDevice = obj.mDevice
    lIRIG = obj.mIRIG

    lTimebase=TimestampTimebase.__members__[timebase]
    lIRIG.set_ts_timebase(lTimebase)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@irig.command('set-seconds-offset', short_help="Set ts seconds offset")
@click.pass_obj
@click.argument('offset', type=click.IntRange(-128, 127))
def set_seconds_offset(obj, offset):

    lDevice = obj.mDevice
    lIRIG = obj.mIRIG

    lIRIG.set_ts_seconds_offset(offset)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@irig.command('set-ticks-offset', short_help="Set ts ticks offset")
@click.pass_obj
@click.argument('offset', type=click.IntRange(-32768, 32767))
def set_ticks_offset(obj, offset):

    lDevice = obj.mDevice
    lIRIG = obj.mIRIG

    lIRIG.set_ts_ticks_offset(offset)
# ------------------------------------------------------------------------------