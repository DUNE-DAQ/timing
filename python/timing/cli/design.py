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
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignOverlord
from timing.common.definitions import kBoardNameMap, kCarrierNameMap, kDesignNameMap
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

#    echo("Design '{}' on board '{}' on carrier '{}' with frequency {} MHz".format(
#        style(kDesignNameMap[lBoardInfo['design_type'].value()], fg='blue'),
#        style(kBoardNameMap[lBoardInfo['board_type'].value()], fg='blue'),
#        style(kCarrierNameMap[lBoardInfo['carrier_type'].value()], fg='blue'),
#        style(str(lBoardInfo['clock_frequency'].value()/1e6), fg='blue')
#    ))

 #   if lBoardInfo['board_type'].value() in kLibrarySupportedBoards and lBoardInfo['design_type'].value() in kLibrarySupportedDesigns:
 #       lVersion = lTopDesign.read_firmware_version()
 #       lTopDesign.validate_firmware_version()

#        try:
#            echo(lDevice.getNode('io').get_hardware_info())
#        except:
#            secho("Failed to retrieve hardware information! I2C issue? Initial board reset needed?", fg='yellow')
#            e = sys.exc_info()[0]
#            secho("Error: {}".format(e), fg='red')
#
#    echo("FW rev: {}".format(
#        style(format_firmware_version(lVersion), fg='cyan'),
#    ))

    obj.mDevice = lDevice
    obj.mTopDesign = lTopDesign

 #   obj.mBoardType = lBoardInfo['board_type'].value()
 #   obj.mCarrierType = lBoardInfo['carrier_type'].value()
 #   obj.mDesignType = lBoardInfo['design_type'].value()    
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
    lTopDesign.switch_cdr_mux(mux)
    active_mux=lTopDesign.read_active_cdr_mux()

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