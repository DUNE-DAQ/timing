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
@click.argument('phase', type=toolbox.IntRange(0x0,0xfff))
@click.pass_obj
@click.pass_context
def applydelay(ctx, obj, addr, cdelay, phase):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lTopDesign = obj.mTopDesign
    lMaster = obj.mMaster

    lMaster.apply_endpoint_delay(addr, cdelay, phase)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('measure-rtt', short_help="Measure endpoint round trip time")
@click.argument('addr', type=toolbox.IntRange(0x0,0xffff))
@click.option('--fanout-ept-address', '-a', type=click.IntRange(0,65535), help='fanout endpoint address')
@click.option('--fanout-mux', '-f', type=click.IntRange(0,7), help='Mux select (fanout only)')
@click.option('--sfp-control/--no-sfp-control', default=True, help='Control SFP or not')
@click.pass_obj
@click.pass_context
def measure_rtt(ctx, obj, addr, fanout_ept_address, fanout_mux, sfp_control):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lTopDesign = obj.mTopDesign
    lMaster = obj.mMaster

    rtt=0
    if fanout_mux is not None and fanout_ept_address is not None:
        rtt=lMaster.measure_endpoint_rtt(addr, fanout_ept_address, fanout_mux, sfp_control)
    elif fanout_mux is not None and fanout_ept_address is None:
        raise RuntimeError('Fanout mux slot provided, but no fanout endpoint address, please provide the fanout endpoint address')
    elif fanout_mux is None and fanout_ept_address is not None:
        raise RuntimeError('Fanout endpoint address provided, but no fanout mux slot, please provide the fanout mux slot')
    else:
        rtt=lMaster.measure_endpoint_rtt(addr, sfp_control)
    echo(f"RTT: {rtt} endpoint adr: {addr} fanout mux: {fanout_mux} fanout ept address: {fanout_ept_address}")
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
#@align.command('scan-mux', short_help="Scan SFP mux for transmitting SFPs")
#@click.pass_obj
#def scanmux(obj):
#
#    lDevice = obj.mDevice
#    lTopDesign = obj.mTopDesign
#    lBoardType = obj.mBoardType
#
#    if lBoardType in [kBoardPC059, kBoardFIB]:
#        lTopDesign.scan_sfp_mux()
#    else:
#        raise RuntimeError('Mux scan is only available on MUX boards')
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
    if lBoardType in [kBoardPC059, kBoardFIB]: #TODO: don't include fib v2
        if mux is not None:
            lTopDesign.switch_mux(mux)
            lMaster.enable_upstream_endpoint()
        else:
            raise RuntimeError('MUX board: please supply an SFP mux channel')
    else:
        lMaster.enable_upstream_endpoint()
# ------------------------------------------------------------------------------
