from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import math
import timing
import traceback
import sys
import random
from io import StringIO

# PDT imports
import timing.cli.toolbox as toolbox
import timing.common.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars, basename
from timing.core import SI534xSlave, I2CExpanderSlave

from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignOverlord, kDesignGaia, kDesignBoreas, kDesignKerberos, kDesignChronos, kDesignFanout
from timing.common.definitions import kBoardNameMap, kCarrierNameMap, kDesignNameMap, TimestampTimebase, TimestampSource, ClockSource, kFreeRun, kInput0, kInput1, kInput2, kInput3, kUpstream, kSoftware
from timing.common.definitions import kLibrarySupportedBoards, kLibrarySupportedDesigns

from timing.common.toolbox import format_firmware_version
# ------------------------------------------------------------------------------
#    __  ___         __         
#   /  |/  /__ ____ / /____ ____
#  / /|_/ / _ `(_-</ __/ -_) __/
# /_/  /_/\_,_/___/\__/\__/_/   
                        

@click.group('design', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, shell_complete=toolbox.completeDevices)
def design(obj, device):
    '''
    Timing master commands.

    DEVICE: uhal device identifier
    '''
    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)
        
    echo('Created device ' + click.style(lDevice.id(), fg='blue'))

    lTopDesign = lDevice.getNode('')
    
    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lDevice.dispatch()

    echo("Design '{}' on board '{}' on carrier '{}' with frequency {} MHz".format(
        style(kDesignNameMap[lBoardInfo['design_type'].value()], fg='blue'),
        style(kBoardNameMap[lBoardInfo['board_type'].value()], fg='blue'),
        style(kCarrierNameMap[lBoardInfo['carrier_type'].value()], fg='blue'),
        style(str(lBoardInfo['clock_frequency'].value()/1e6), fg='blue')
    ))

    if lBoardInfo['board_type'].value() in kLibrarySupportedBoards and lBoardInfo['design_type'].value() in kLibrarySupportedDesigns:
        lVersion = lTopDesign.read_firmware_version()
        lTopDesign.validate_firmware_version()

        try:
            echo(lDevice.getNode('io').get_hardware_info())
        except:
            secho("Failed to retrieve hardware information! I2C issue? Initial board reset needed?", fg='yellow')
            e = sys.exc_info()[0]
            secho("Error: {}".format(e), fg='red')

    echo("FW rev: {}".format(
        style(format_firmware_version(lVersion), fg='cyan'),
    ))

    obj.mDevice = lDevice
    obj.mTopDesign = lTopDesign

    obj.mBoardType = lBoardInfo['board_type'].value()
    obj.mCarrierType = lBoardInfo['carrier_type'].value()
    obj.mDesignType = lBoardInfo['design_type'].value()    
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@design.command('status', short_help="Print master status")
@click.pass_obj
def status(obj):
    
    lTopDesign = obj.mTopDesign
    echo(lTopDesign.get_status())
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@design.command('cdr-switch', short_help="switch upstream CDR")
@click.argument('mux', type=int)
@click.pass_obj
def cdrswitch(obj, mux):

    lTopDesign = obj.mTopDesign
    lTopDesign.switch_mux(mux)
    active_mux=lTopDesign.read_active_mux()

    echo(f"cdr mux set to {active_mux}")
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@design.command('cdr-status', short_help="Print upstream CDR status")
@click.option('--id', type=int)
@click.pass_obj
@click.pass_context
def cdrstatus(ctx, obj, id):

    if id is None:
        cdr_node = obj.mDevice.getNode('cdr')
    else:
        cdr_node = obj.mDevice.getNode(f"cdr{id}")

    echo(cdr_node.get_status())
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@design.command('cdr-resync', short_help="Resync upstream CDR")
@click.option('--id', type=int)
@click.pass_obj
@click.pass_context
def cdrresync(ctx, obj, id):

    if id is None:
        cdr_node = obj.mDevice.getNode('cdr')
    else:
        cdr_node = obj.mDevice.getNode(f"cdr{id}")

    cdr_node.resync()

    time.sleep(0.5)

    ctx.forward(cdrstatus)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@design.command('switch-timing-source', short_help="switch timing source")
@click.argument('source', type=int)
@click.pass_obj
def cdrswitch(obj, source):

    lTopDesign = obj.mTopDesign
    lTopDesign.switch_timing_source_mux(source)
    active_source=lTopDesign.read_active_timing_source_mux()

    echo(f"timing source mux set to {active_source}")
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@design.command('configure', short_help="configure a whole design")
@click.option('--clock-source', type=click.Choice(ClockSource.__members__.keys()))
@click.option('--ts-source', type=click.Choice(TimestampSource.__members__.keys()))
@click.option('--timebase', type=click.Choice(TimestampTimebase.__members__.keys()))
@click.pass_obj
def configure(obj, clock_source, ts_source, timebase):

    lTopDesign = obj.mTopDesign
    lDesignType = obj.mDesignType
    lDesignName=kDesignNameMap[lDesignType]

    # user convenience
    if clock_source is None:
        if lDesignType in [kDesignMaster, kDesignBoreas, kDesignOuroboros, kDesignOuroborosSim]:
            lClockSource=kFreeRun
            lTimestampSource=kSoftware
        elif lDesignType in [kDesignEndpoint, kDesignChronos, kDesignFanout]:
            lClockSource=kInput1
        elif lDesignType in [kDesignGaia, kDesignKerberos]:
            lClockSource=kInput0
            lTimestampSource=kUpstream
        else:
            secho("Unable to match a default clock source for design {}.\nConfigure failed!".format(lDesignName), fg='red')
            return
    else:
        lClockSource=ClockSource.__members__[clock_source]

    if ts_source is None:
        if lDesignType in [kDesignMaster, kDesignBoreas, kDesignOuroboros, kDesignOuroborosSim]:
            lTimestampSource=kSoftware
        elif lDesignType in [kDesignGaia, kDesignKerberos]:
            lTimestampSource=kUpstream
        elif lDesignType not in [kDesignEndpoint, kDesignChronos, kDesignFanout]:
            secho("Unable to match a default timestamp source for design {}.\nConfigure failed!".format(lDesignName), fg='red')
            return
    else:
        lTimestampSource=TimestampSource.__members__[ts_source]

    if lDesignType in [kDesignMaster, kDesignBoreas, kDesignGaia, kDesignKerberos]:
        if lDesignType == kDesignGaia:
            if timebase is not None:
                lTimebase=TimestampTimebase.__members__[timebase]
                lTopDesign.configure(lClockSource, lTimestampSource, lTimebase)
            else:
                secho("Supply ts timebase option for design Gaia!", fg='red')
        else:
            lTopDesign.configure(lClockSource, lTimestampSource)
    elif lDesignType in [kDesignEndpoint, kDesignChronos, kDesignFanout]:
        lTopDesign.configure(lClockSource)
    else:
        secho("Configure not supported for design {}.\nConfigure failed!".format(lDesignName), fg='red')
# ------------------------------------------------------------------------------