from __future__ import print_function
import click
import time
from io import StringIO
import traceback

# PDT imports
import timing.cli.toolbox as toolbox
import timing.common.definitions as defs

from click import echo, style, secho
from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU, kBoardFIB, kBoardMIB
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout
from timing.common.definitions import kBoardNameMap, kCarrierNameMap, kDesignNameMap
from timing.common.definitions import kLibrarySupportedBoards

from .master import master

# ------------------------------------------------------------------------------
@master.group('align', short_help='Endpoint alignment command group.')
@click.pass_obj
def align(obj):
    lDevice = obj.mDevice
    lMaster = obj.mMaster
    obj.mGlobal = lMaster.getNode('global')
    #obj.mACmd = lMaster.getNode('acmd')
    #obj.mEcho = lMaster.getNode('echo')
    #bj.mIO = lDevice.getNode('io')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('apply-delay', short_help="Send delay adjust command endpoint")
@click.argument('addr', type=toolbox.IntRange(0x0,0xffff))
@click.argument('cdelay', type=toolbox.IntRange(0x0,0xf))
@click.argument('fdelay', type=toolbox.IntRange(0x0,0xfff))
@click.option('--mux', '-m', type=click.IntRange(0,12), help='Mux select')
@click.option('--force', '-f', is_flag=True, default=False, help='Skip RTT measurement')
@click.pass_obj
@click.pass_context
def applydelay(ctx, obj, addr, cdelay, fdelay, mux, force):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lTopDesign = obj.mTopDesign

    # Are we working with a board with a return path mux (i.e. a fanout board)?
    if lBoardType in [kBoardPC059, kBoardFIB]:
        if mux is None:
            if force == True:
                lTopDesign.apply_endpoint_delay(addr, cdelay, fdelay, 0, not force, True, 0)
            else:
                raise RuntimeError('MUX board: please supply an SFP mux channel')
        else:
            lTopDesign.apply_endpoint_delay(addr, cdelay, fdelay, 0, not force, True, mux)
            
    else:
        lTopDesign.apply_endpoint_delay(addr, cdelay, fdelay, 0, not force, True)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('measure-delay', short_help="Measure endpoint round trip time")
@click.argument('addr', type=toolbox.IntRange(0x0,0xffff))
@click.option('--mux', '-m', type=click.IntRange(0,12), help='Mux select (fanout only)')
@click.option('--sfp-control/--no-sfp-control', default=True, help='Control SFP or not')
@click.pass_obj
@click.pass_context
def measuredelay(ctx, obj, addr, mux, sfp_control):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lTopDesign = obj.mTopDesign
    
    # or a different type of fanout board
    if lBoardType in [kBoardPC059, kBoardFIB] and sfp_control == True:
        if mux is not None:
            echo("Endpoint (adr: {}, mux: {}) RTT: {}".format(addr,mux,lTopDesign.measure_endpoint_rtt(addr, sfp_control, mux)))
        else:
            raise RuntimeError('MUX board: please supply an SFP mux channel')
    else:
        echo("Endpoint (adr: {}) RTT: {}".format(addr,lTopDesign.measure_endpoint_rtt(addr, sfp_control)))
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('toggle-tx', short_help="Control the endpoint SFP Tx laser")
@click.argument('addr', type=toolbox.IntRange(0x0,0xffff))
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

    if lBoardType in [kBoardPC059, kBoardFIB]:
        lTopDesign.scan_sfp_mux()
    else:
        raise RuntimeError('Mux scan is only available on MUX boards')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('switch-n-lock', short_help="Wait for RTT endpoint to become ready")
@click.option('--mux', '-m', type=click.IntRange(0,12), help='Mux select (fanout only)')
@click.pass_obj
def switchnlock(obj, mux):
    
    lDevice = obj.mDevice
    lTopDesign = obj.mTopDesign
    lBoardType = obj.mBoardType
    lMaster = obj.mMaster
    
    # or a different type of fanout board
    if lBoardType in [kBoardPC059, kBoardFIB, kBoardMIB]:
        if mux is not None:
            lTopDesign.switch_downstream_mux_channel(mux, True)
        else:
            raise RuntimeError('MUX board: please supply an downstream mux channel')
    else:
        lMaster.enable_upstream_endpoint()
# ------------------------------------------------------------------------------
