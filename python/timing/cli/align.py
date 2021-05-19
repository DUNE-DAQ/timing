from __future__ import print_function
import click
import time
from io import StringIO
import traceback

# PDT imports
import timing.cli.toolbox as toolbox
import timing.common.definitions as defs

from click import echo, style, secho
from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout
from timing.common.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap
from timing.common.definitions import kLibrarySupportedBoards

from .master import master

# ------------------------------------------------------------------------------
@master.group('align', short_help='Endpoint alignment command group.')
@click.pass_obj
def align(obj):
    lDevice = obj.mDevice
    lMaster = obj.mMaster
    obj.mGlobal = lMaster.getNode('global')
    obj.mACmd = lMaster.getNode('acmd')
    obj.mEcho = lMaster.getNode('echo')
    obj.mIO = lDevice.getNode('io')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('apply-delay', short_help="Send delay adjust command endpoint")
@click.argument('addr', type=toolbox.IntRange(0x0,0x100))
@click.argument('cdelay', type=toolbox.IntRange(0x0,0x32))
@click.argument('fdelay', type=toolbox.IntRange(0x0,0xf))
@click.option('--mux', '-m', type=click.IntRange(0,7), help='Mux select')
@click.option('--force', '-f', is_flag=True, default=False, help='Skip RTT measurement')
@click.pass_obj
@click.pass_context
def applydelay(ctx, obj, addr, cdelay, fdelay, mux, force):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lTopDesign = obj.mTopDesign

    # or a different type of fanout board
    if lBoardType == kBoardPC059:
        if mux is None:
            if force == True:
                lTopDesign.apply_endpoint_delay(addr, cdelay, fdelay, 0, not force, True, 0)
            else:
                raise RuntimeError('PC059 board: please supply an SFP mux channel')
        else:
            lTopDesign.apply_endpoint_delay(addr, cdelay, fdelay, 0, not force, True, mux)
            
    else:
        lTopDesign.apply_endpoint_delay(addr, cdelay, fdelay, 0, not force, True)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('measure-delay', short_help="Measure endpoint round trip time")
@click.argument('addr', type=toolbox.IntRange(0x0,0x100))
@click.option('--mux', '-m', type=click.IntRange(0,7), help='Mux select (fanout only)')
@click.pass_obj
@click.pass_context
def measuredelay(ctx, obj, addr, mux):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lTopDesign = obj.mTopDesign
    
    # or a different type of fanout board
    if lBoardType == kBoardPC059:
        if mux is not None:
            echo("Endpoint (adr: {}, mux: {}) RTT: {}".format(addr,mux,lTopDesign.measure_endpoint_rtt(addr, True, mux)))
        else:
            raise RuntimeError('PC059 board: please supply an SFP mux channel')
    else:
        echo("Endpoint (adr: {}) RTT: {}".format(addr,lTopDesign.measure_endpoint_rtt(addr, True)))
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('toggle-tx', short_help="Control the endpoint SFP Tx laser")
@click.argument('addr', type=toolbox.IntRange(0x0,0x100))
@click.option('--on/--off', default=True, help='enable/disable tx')
@click.pass_obj
def toggletx(obj, addr, on):

    lMaster = obj.mMaster
    lMaster.switch_endpoint_sfp(addr, on)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('scan-mux', short_help="Scan SFP mux for transmitting SFPs")
@click.pass_obj
def scanmux(obj):

    lDevice = obj.mDevice
    lTopDesign = obj.mTopDesign
    lBoardType = obj.mBoardType

    if lBoardType == kBoardPC059:
        lTopDesign.scan_sfp_mux()
    else:
        raise RuntimeError('Mux scan is only available on PC059 boards')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('switch-n-lock', short_help="Wait for RTT endpoint to become ready")
@click.option('--mux', '-m', type=click.IntRange(0,7), help='Mux select (fanout only)')
@click.pass_obj
def switchnlock(obj, mux):
    
    lDevice = obj.mDevice
    lTopDesign = obj.mTopDesign
    lBoardType = obj.mBoardType
    lMaster = obj.mMaster
    
    # or a different type of fanout board
    if lBoardType == kBoardPC059:
        if mux is not None:
            lTopDesign.switch_sfp_mux_channel(mux, True)
        else:
            raise RuntimeError('PC059 board: please supply an SFP mux channel')
    else:
        lMaster.enable_upstream_endpoint()
# ------------------------------------------------------------------------------
