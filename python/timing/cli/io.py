from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import math
import sys
import timing

import timing.cli.toolbox as toolbox
import timing.common.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars, basename
from timing.core import SI534xSlave, I2CExpanderSlave, DACSlave

from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU, kBoardFIB, kBoardMIB, kBoardPC069, kBoardGIB
from timing.common.definitions import kFMCRev1, kFMCRev2, kFMCRev3, kFMCRev4, kPC059Rev1, kTLURev1, kSIMRev1, kFIBRev1, kMIBRev1, kGIBRev1
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed, kCarrierNexusVideo, kCarrierTrenzTE0712
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignChronos, kDesignBoreas, kDesignTest, kDesignKerberos, kDesignGaia
from timing.common.definitions import kBoardNameMap, kCarrierNameMap, kDesignNameMap, kUIDRevisionMap, kClockConfigMap
from timing.common.definitions import kLibrarySupportedBoards, kLibrarySupportedDesigns

# ------------------------------------------------------------------------------
#    __  ___         __         
#   /  |/  /__ ____ / /____ ____
#  / /|_/ / _ `(_-</ __/ -_) __/
# /_/  /_/\_,_/___/\__/\__/_/   
                        

@click.group('io', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, shell_complete=toolbox.completeDevices)
def io(obj, device):
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

    if lBoardInfo['board_type'].value() in kLibrarySupportedBoards and lBoardInfo['design_type'].value() in kLibrarySupportedDesigns:
        lTopDesign.validate_firmware_version()

    echo("Design '{}' on board '{}' on carrier '{}' with frequency {} MHz".format(
        style(kDesignNameMap[lBoardInfo['design_type'].value()], fg='blue'),
        style(kBoardNameMap[lBoardInfo['board_type'].value()], fg='blue'),
        style(kCarrierNameMap[lBoardInfo['carrier_type'].value()], fg='blue'),
        style(str(lBoardInfo['clock_frequency'].value()/1e6), fg='blue')
    ))

    obj.mDevice = lDevice
    
    obj.mBoardType = lBoardInfo['board_type'].value()
    obj.mCarrierType = lBoardInfo['carrier_type'].value()
    obj.mDesignType = lBoardInfo['design_type'].value()
    
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('reset', short_help="Perform a hard reset on the timing master.")
@click.option('--soft', '-s', is_flag=True, default=False, help='Soft reset i.e. skip the clock chip configuration.')
@click.option('--fanout-mode', 'fanout', type=click.IntRange(0, 1), default=0, help='Configures the board in fanout mode (pc059 only)')
@click.option('--downstream-mux-sel', 'downstream_mux_sel', type=int, help='Configures the downstream mux on the fib/mib/pc059')
@click.option('--force-pll-cfg', 'forcepllcfg', type=click.Path(exists=True))
@click.pass_obj
@click.pass_context
def reset(ctx, obj, soft, fanout, downstream_mux_sel, forcepllcfg):
    '''
    Perform a hard reset on the timing master, including

    \b
    - ipbus registers
    - i2c buses
    - pll and pll configuration

    \b
    Fanout mode:
    0 = sfp - fanout mode
    1 = local master - standalone mode
    '''

    echo('Resetting ' + click.style(obj.mDevice.id(), fg='blue'))

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lDesignType = obj.mDesignType

    lIO = lDevice.getNode('io')

    lPLLConfigFilePath=""
    if forcepllcfg is not None:
        lPLLConfigFilePath = forcepllcfg

    if lBoardType in kLibrarySupportedBoards:

        if soft:
            lIO.soft_reset()
            return
        
        if lDesignType == kDesignFanout:
            
            if fanout == 0:
                secho("Fanout mode enabled", fg='green')
            else:
                secho("local master: standalone mode", fg='green')

            lIO.reset(fanout, lPLLConfigFilePath)
            lDevice.getNode('switch').configure_master_source(fanout)
            
            if lBoardType in [ kBoardPC059, kBoardFIB, kBoardMIB ] and downstream_mux_sel is not None:
                lIO.switch_downstream_mux_channel(downstream_mux_sel)
                secho("Active downstream mux " + hex(downstream_mux_sel), fg='cyan')
        else:
            lIO.reset(lPLLConfigFilePath)
    
        ctx.invoke(clkstatus)

    else:
        secho("Board identifier {} not supported by timing library".format(lBoardType), fg='yellow')
        # board not supported by library, do reset here
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('freq', short_help="Measure some frequencies.")
@click.pass_obj
def freq(obj):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')
    
    secho("PLL Clock frequency measurement:", fg='cyan')
    
    if lBoardType in kLibrarySupportedBoards:    
        echo(lIO.get_clock_frequencies_table())
    else:
        secho("Board {} not supported by timing library".format(lBoardType), fg='yellow')
        # do freq measurement here
        if lBoardType == kBoardMIB:
            
            # MIB clock freq meas reg ops here

            secho('MIB clock freqs', fg='green')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('status')
@click.pass_obj
@click.pass_context
@click.option('-v', 'verbose', is_flag=True)
def status(ctx, obj, verbose):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')

    if lBoardType in kLibrarySupportedBoards:
        ctx.invoke(print_hardware_info)
        echo(lIO.get_status())
    else:
        secho("Board {} not supported by timing library".format(lBoardType), fg='yellow')
        # do status printing here
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('clk-status')
@click.pass_obj
@click.pass_context
@click.option('-v', 'verbose', is_flag=True)
def clkstatus(ctx, obj, verbose):

    lDevice = obj.mDevice
    lDesignType = obj.mDesignType
    lIO = lDevice.getNode('io')
    lBoardType = obj.mBoardType
    
    ctx.invoke(status)

    if lBoardType in [kBoardPC059, kBoardFIB]:
        mux_fib = lIO.read_active_downstream_mux_channel()
        secho("Active sfp mux {} ".format(mux_fib))

    echo()
    ctx.invoke(freq)
    echo()

    if lBoardType in kLibrarySupportedBoards:    
        echo(lIO.get_pll_status())
    else:
        secho("Board {} not supported by timing library".format(lBoardType), fg='yellow')
        # do freq measurement here
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('dac-setup')
@click.argument('value', type=click.IntRange(0,0xffff))
@click.pass_obj
@click.pass_context
def dacsetup(ctx, obj, value):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')

    if lBoardType == kBoardTLU:
        lIO.configure_dac(0,value)
        lIO.configure_dac(1,value)
        secho("DAC1 and DAC2 set to " + hex(value), fg='cyan')
    else:
        secho("DAC setup only supported for TLU")
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('sfp-status', short_help="Read SFP parameters.")
@click.pass_obj
@click.pass_context
@click.option('--sfp-id', 'sfp_id', required=False, type=click.IntRange(0, 8), help='SFP id to query.')
def sfpstatus(ctx, obj, sfp_id):
    '''
    Read SFP status
    '''

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lCarrierType = obj.mCarrierType
    lDesignType = obj.mDesignType

    lIO = lDevice.getNode('io')
    
    
    if lBoardType in kLibrarySupportedBoards:
        ctx.invoke(print_hardware_info)
        if sfp_id is not None:
            echo(lIO.get_sfp_status(sfp_id))
        else:
            if lBoardType in [kBoardFMC, kBoardTLU, kBoardPC069]:
                echo(lIO.get_sfp_status(0))
            elif lBoardType in [ kBoardPC059, kBoardFIB, kBoardMIB ]:
                # PC059 sfp id 0 is upstream sfp
                if lBoardType == kBoardPC059:
                    lSFPIDRange = 9
                elif lBoardType == kBoardFIB:
                    lSFPIDRange = 8
                elif lBoardType == kBoardMIB:
                    lSFPIDRange = 3
                elif lBoardType == kBoardGIB:
                    lSFPIDRange = 6
                for i in range(lSFPIDRange):
                    try:
                        echo(lIO.get_sfp_status(i))
                        #echo()
                    except:
                        secho(f"SFP {i} status gather failed\n", fg='red')
                        pass

    else:
        secho("Board {} not supported by timing library".format(lBoardType), fg='yellow')
        # do sfp status here
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@io.command('switch-sfp-tx', short_help="Control sfp tx")
@click.pass_obj
@click.pass_context
@click.option('--sfp-id', 'sfp_id', required=False, type=click.IntRange(0, 8), help='SFP id to query.')
@click.option('--on/--off', default=False, help='enable/disable tx; default: FALSE')
def switchsfptx(ctx, obj, sfp_id, on):
    '''
    Toggle SFP tx disable reg (if supported)
    '''

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lCarrierType = obj.mCarrierType
    lDesignType = obj.mDesignType

    lIO = lDevice.getNode('io') 

    if lBoardType in kLibrarySupportedBoards:
        ctx.invoke(print_hardware_info)
        if sfp_id is not None:
            lIO.switch_sfp_soft_tx_control_bit(sfp_id, on)
            echo(lIO.get_sfp_status(sfp_id))
        else:
            if lBoardType == kBoardFMC or lBoardType == kBoardTLU:
                lIO.switch_sfp_soft_tx_control_bit(0, on)
                echo(lIO.get_sfp_status(0))
            elif ( lBoardType == kBoardPC059 ):
                for i in range(9):
                    lIO.switch_sfp_soft_tx_control_bit(i, on)
                    echo(lIO.get_sfp_status(i))
    else:
        secho("Board {} not supported by timing library".format(lBoardType), fg='yellow')
        # do sfp switch here
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@io.command('switch-downstream-mux', short_help="Switch downstream mux")
@click.argument('mux', type=int)
@click.pass_obj
def switchdownstreammux(obj, mux):
    
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')

    if lBoardType in [kBoardPC059, kBoardFIB, kBoardMIB]:
        
        echo("Setting downstream mux channel: {}".format(mux))
        lIO.switch_downstream_mux_channel(mux)

    else:
        raise RuntimeError('Board {} does not have a downstream mux!'.format(kBoardNameMap[lBoardType]))
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@io.command('switch-upstream-mux', short_help="Switch upstream mux")
@click.argument('mux', type=int)
@click.pass_obj
def switchupstreammux(obj, mux):
    
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')

    if lBoardType == kBoardMIB:
        echo("Setting upstream mux channel: {}".format(mux))
        lIO.switch_upstream_mux_channel(mux)
    else:
        raise RuntimeError('Board {} does not have/support an upstream mux!'.format(kBoardNameMap[lBoardType]))
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@io.command('read-upstream-mux', short_help="Read active upstream mux")
@click.pass_obj
def switchupstreammux(obj):
    
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')

    if lBoardType == kBoardMIB:
        active_mux = lIO.read_active_upstream_mux_channel()
        echo("Active upstream mux channel: {}".format(active_mux))
    else:
        raise RuntimeError('Board {} does not have/support an upstream mux!'.format(kBoardNameMap[lBoardType]))
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@io.command('print-hardware-info')
@click.pass_obj
@click.pass_context
def print_hardware_info(ctx, obj):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')

    if lBoardType in kLibrarySupportedBoards:
        try:
            echo(lIO.get_hardware_info())
        except:
            secho("Failed to retrieve hardware information! I2C issue? Initial board reset needed?", fg='yellow')
            e = sys.exc_info()[0]
            secho("Error: {}".format(e), fg='red')
    else:
        secho("Board {} not supported by timing library".format(lBoardType), fg='yellow')
        # do status printing here
# ------------------------------------------------------------------------------