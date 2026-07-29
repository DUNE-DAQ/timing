from __future__ import print_function
import click
import time
from io import StringIO
import traceback

# PDT imports
import timing.cli.toolbox as toolbox
import timing.common.definitions as defs

from click import echo, style, secho
from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU, kBoardFIB, kBoardMIB, kBoardGIB
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
@click.option('--sfp-control/--no-sfp-control', default=True, help='Control SFP or not')
@click.pass_obj
@click.pass_context
def measure_rtt(ctx, obj, addr, sfp_control):

    lMaster = obj.mMaster

    rtt=lMaster.measure_endpoint_rtt(addr, sfp_control)
    echo(f"Endpoint ({addr}) RTT: {rtt}")
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@align.command('set-deskew-done', short_help="Set the endpoint deskew flag high")
@click.argument('addr', type=toolbox.IntRange(0x0,0xffff))
@click.pass_obj
def seteptdeskewdone(obj, addr):

    lMaster = obj.mMaster
    lMaster.set_endpoint_deskew_done(addr)
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
@align.command('resync-ept', short_help='Resync an endpoint.')
@click.pass_obj
@click.argument('address', type=toolbox.IntRange(0x0,0xffff))
def resyncept(obj, address):
    '''
    Resync an endpoint
    '''

    lMaster = obj.mMaster
    lMaster.resync_endpoint(address)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@align.command('deskew-ept', short_help="Align endpoint")
@click.argument('addr', type=toolbox.IntRange(0x0,0xffff))
@click.argument('cdelay', type=toolbox.IntRange(0x0,0xf))
@click.argument('phase', type=toolbox.IntRange(0x0,0xfff))
@click.option('--sfp-control/--no-sfp-control', default=True, help='Control SFP or not')
@click.option('--skip-done', is_flag=True, default=False, help='Do not set deskew done flag in the endpoint.')
@click.pass_obj
@click.pass_context
def deskewept(ctx, obj, addr, cdelay, phase, sfp_control, skip_done):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lTopDesign = obj.mTopDesign
    lMaster = obj.mMaster

    if (sfp_control):
        # turn on sfp if controlling it
        lMaster.setup_endpoint_rx(addr)
    else:
        # only reset mst rx if not controlling sfp
        lMaster.enable_upstream_endpoint()

    # first measure
    rtt_before_deskew=lMaster.measure_endpoint_rtt(addr, False)
    echo(f"Endpoint ({addr}) RTT before deskew: {rtt_before_deskew}")

    lMaster.apply_endpoint_delay(addr, cdelay, phase)

    ## reset mst cdr after adjust
    if (sfp_control):
        lMaster.enable_upstream_endpoint()

    # re measure after adjust
    rtt_after_deskew=lMaster.measure_endpoint_rtt(addr, False)
    echo(f"Endpoint ({addr}) RTT after deskew:  {rtt_after_deskew}")

    if not skip_done:
        lMaster.set_endpoint_deskew_done(addr)

    if (sfp_control):
        lMaster.switch_endpoint_sfp(addr, False)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@align.command('scan-mux', short_help="Scan SFP mux for transmitting SFPs")
@click.pass_obj
def scanmux(obj):

    lDevice = obj.mDevice
    lTopDesign = obj.mTopDesign
    lBoardType = obj.mBoardType

    # TODO: add bindings
    if lBoardType in [kBoardPC059, kBoardFIB]:
        lTopDesign.scan_mux()
    else:
        raise RuntimeError('Mux scan is only available on MUX boards')
# ------------------------------------------------------------------------------