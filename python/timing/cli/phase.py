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
@click.group('phase', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, shell_complete=toolbox.completeDevices)
@click.argument('id', type=int)
def phase(obj, id, device):
    '''
    Phase commands.

    \b
    DEVICE: uhal device identifier
    '''

    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)

    echo('Created phase device ' + style(lDevice.id(), fg='blue'))
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

    # Ensure that target endpoint exists
    lPhaseNames = lDevice.getNodes(f"phase{id}")

    if len(lPhaseNames) == 0:
        raise click.ClickException(f"Phase {id} does not exist")
    elif len(lPhaseNames) > 1:
        raise click.ClickException(f"Multiple phase {id} matches")

    obj.mDevice = lDevice
    obj.mPhase = lDevice.getNode(f"phase{id}")
    obj.mIO = lDevice.getNode('io')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@phase.command('measure-phase', short_help="Measure phase.")
@click.option('--ref-clk', type=int, default=255, help="Select reference clock, 0 : nominal, 1 : 22.5 phase to nominal, 255 : auto")
@click.pass_obj
def measure_phase(obj, ref_clk):
    phase_node = obj.mPhase

    secho("Phase measurement:", fg='cyan')
    # Measure the generated clock frequency
    phase = phase_node.measure_phase(ref_clk)

    echo(f"Phase [ns] : {phase}")
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@phase.command('measure-phases', short_help="Measure phase.")
@click.pass_obj
def measure_phases(obj):
    phase_node = obj.mPhase

    secho("Phases measurement:", fg='cyan')
    # Measure the generated clock frequency
    phases = phase_node.measure_phases()

    echo(f"Phase clk 0 [ns] : {phases[0]}, clk 1 [ns] : {phases[1]}")
# ------------------------------------------------------------------------------