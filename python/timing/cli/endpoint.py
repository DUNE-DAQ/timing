import click
import sys

import collections

from . import toolbox
import timing.common.definitions as defs
from timing.common.definitions import kLibrarySupportedBoards, kLibrarySupportedDesigns
from timing.common.toolbox import format_firmware_version

from click import echo, style, secho
import time

# ------------------------------------------------------------------------------
#    ____        __          _      __ 
#   / __/__  ___/ /__  ___  (_)__  / /_
#  / _// _ \/ _  / _ \/ _ \/ / _ \/ __/
# /___/_//_/\_,_/ .__/\___/_/_//_/\__/ 
#              /_/                     
@click.group('ept', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, shell_complete=toolbox.completeDevices)
def endpoint(obj, device):
    '''
    Endpoint master commands.

    \b
    DEVICE: uhal device identifier
    '''

    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)

    echo('Created endpoint device ' + style(lDevice.id(), fg='blue'))
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

    obj.mDevice = lDevice
    obj.mEndpoint = lDevice.getNode('endpoint0')
    obj.mIO = lDevice.getNode('io')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@endpoint.command('freq', short_help="Measure some frequencies.")
@click.pass_obj
def freq(obj):
    lEndPointNode = obj.mEndpoint

    secho("Endpoint frequency measurement:", fg='cyan')
    # Measure the generated clock frequency
    freq = ep.read_clock_frequency()

    echo( "Endpoint freq MHz : {}".format(freq) )
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@endpoint.command('enable')
@click.argument('action', default='on', type=click.Choice(['on', 'off', 'reset']))
@click.option('--address', '-a', type=toolbox.IntRange(0x0,0x100), help='Address', default=0)
@click.pass_obj
@click.pass_context
def enable(ctx, obj, action, address):
    '''
    Activate timing endpoint wrapper block.
    '''

    lEndPointNode = obj.mEndpoint
    if action == 'off':
        lEndPointNode.disable()
    elif action == 'on':
        lEndPointNode.enable(address)
    elif action == 'reset':
        lEndPointNode.reset(address)

    time.sleep(0.1)
    ctx.invoke(status)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@endpoint.command('status', short_help='Display the status of timing endpoint.')
@click.pass_obj
@click.option('--watch', '-w', is_flag=True, default=False, help='Turn on automatic refresh')
@click.option('--period', '-p', type=click.IntRange(0, 240), default=2, help='Period of automatic refresh')
def status(obj, watch, period):
    '''
    Display the endpoint status, accepted and rejected command counters
    '''
    lNumCtrs = 0x10

    lDevice = obj.mDevice
    lEndPointNode = obj.mEndpoint

    while(True):
        if watch:
            click.clear()
        
        echo ( lEndPointNode.get_status() )

        if watch:
            time.sleep(period)
        else:
            break
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@endpoint.command('readback', short_help='Read the content of the endpoint master readout buffer.')
@click.pass_obj
@click.option('--all/--events', '-a/ ', 'readall', default=False, help="Buffer readout mode.\n- events: only completed events are readout.\n- all: the content of the buffer is fully read-out.")
def readback(obj, readall):
    '''
    Read the content of the endpoint master readout buffer.
    '''
    lDevice = obj.mDevice
    lEndPointNode = obj.mEndpoint
    
    echo(lEndPointNode.get_data_buffer_table(readall))
# ------------------------------------------------------------------------------