import click

import collections

from . import toolbox
import timing.common.definitions as defs
from timing.common.definitions import kLibrarySupportedBoards

from click import echo, style, secho
from .click_texttable import Texttable
import time

# ------------------------------------------------------------------------------
#    ____        __          _      __ 
#   / __/__  ___/ /__  ___  (_)__  / /_
#  / _// _ \/ _  / _ \/ _ \/ / _ \/ __/
# /___/_//_/\_,_/ .__/\___/_/_//_/\__/ 
#              /_/                     
@click.group('hsi', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, autocompletion=toolbox.completeDevices)
def hsi(obj, device):
    '''
    HSI commands.

    \b
    DEVICE: uhal device identifier
    IDS: id(s) of the target endpoint(s).
    '''

    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)

    
    echo('Created hsi device')
    echo(list(lDevice.getNodes()))
    echo(lDevice.id())
    echo(lDevice.uri())

    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lDevice.dispatch()

    if lBoardInfo['board_type'].value() in kLibrarySupportedBoards:
        echo(lDevice.getNode('io').get_hardware_info())
    # Ensure that target endpoint exists

    obj.mDevice = lDevice
    obj.mHSIEndpoint = lDevice.getNode('endpoint0')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@hsi.command('status')
@click.pass_obj
def status(obj):
    '''
    Print the status of CRT endpoint wrapper block.
    '''

    lDevice = obj.mDevice
    lHSIEpt = obj.mHSIEndpoint
    echo(lHSIEpt.get_status())
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@hsi.command('enable')
@click.argument('action', default='on', type=click.Choice(['on', 'off', 'reset']))
@click.option('--partition', '-p', type=click.IntRange(0,4), help='Partition', default=0)
@click.option('--address', '-a', type=toolbox.IntRange(0x0,0x100), help='Address', default=0)
@click.pass_obj
def enable(obj, action, partition, address):
    '''
    Activate timing endpoint wrapper block.
    '''

    lDevice = obj.mDevice
    lHSIEpt = obj.mHSIEndpoint
    
    if action == 'off':
        lHSIEpt.disable()
    elif action == 'on':
        lHSIEpt.enable(partition, address)
    elif action == 'reset':
        lHSIEpt.reset(partition, address)
        lHSIEpt.reset_hsi()

    #time.sleep(0.1)
    #ctx.invoke(status)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@hsi.command('configure', short_help="Configure the HSI endpoint for running")
@click.option('--src', '-s', type=click.IntRange(0x0,0x1), help='Partition', default=0)
@click.option('--re-mask', '-r', type=click.IntRange(0,0xffffffff), help='Partition', default=0)
@click.option('--fe-mask', '-f', type=click.IntRange(0,0xffffffff), help='Partition', default=0)
@click.option('--inv-mask', '-i', type=click.IntRange(0,0xffffffff), help='Partition', default=0)
@click.pass_obj
def configure(obj, src, re_mask, fe_mask, inv_mask):
    '''
    Activate timing endpoint wrapper block.
    '''

    lDevice = obj.mDevice
    lHSIEpt = obj.mHSIEndpoint

    lHSIEpt.reset_hsi()
    lHSIEpt.start_hsi(src, re_mask, fe_mask, inv_mask)
    #lHSIEpt.enable(part,defs.kCommandIDs[pulsecmd])
    #secho("HSI endpoint configured; partition: {}; cmd: {}".format(part, pulsecmd), fg='green')
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@hsi.command('readback', short_help='Read the content of the endpoint master readout buffer.')
@click.pass_obj
@click.option('--all/--events', ' /-a', 'readall', default=False, help="Buffer readout mode.\n- events: only completed events are readout.\n- all: the content of the buffer is fully read-out.")
def readback(obj, readall):
    '''
    Read the content of the endpoint master readout buffer.
    '''
    lDevice = obj.mDevice
    lHSIEpt = obj.mHSIEndpoint
    
    echo(lHSIEpt.get_data_buffer_table(readall))
# ------------------------------------------------------------------------------