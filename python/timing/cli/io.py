from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import math
import timing

import timing.cli.toolbox as toolbox
import timing.common.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars, basename
from timing.core import SI534xSlave, I2CExpanderSlave, DACSlave

from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU, kBoardFIB, kBoardMIB
from timing.common.definitions import kFMCRev1, kFMCRev2, kFMCRev3, kFMCRev4, kPC059Rev1, kTLURev1, kSIMRev1, kFIBRev1, kMIBRev1
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed, kCarrierNexusVideo, kCarrierTrenzTE0712
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignChronos, kDesignBoreas, kDesignTest
from timing.common.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap, kUIDRevisionMap, kClockConfigMap
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
        style(kBoardNamelMap[lBoardInfo['board_type'].value()], fg='blue'),
        style(kCarrierNamelMap[lBoardInfo['carrier_type'].value()], fg='blue'),
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
@click.option('--sfp-mux-sel', 'sfpmuxsel', type=toolbox.IntRange(0x0,0x7), default=0, help='Configures the sfp cdr mux on the fib (downstream) or mib (upstream)')
@click.option('--amc-mux-sel', 'amcmuxsel', type=toolbox.IntRange(0x1,0xc), default=1, help='Configures the amc mux on the mib (downstream)')
@click.option('--force-pll-cfg', 'forcepllcfg', type=click.Path(exists=True))
@click.pass_obj
@click.pass_context
def reset(ctx, obj, soft, fanout, sfpmuxsel, amcmuxsel, forcepllcfg):
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
            
            if lBoardType == kBoardMIB:
                # output to "all" AMCs on
                lDevice.getNode("switch.csr.ctrl.amc_out").write(0xfff)
                
                lDevice.getNode("switch.csr.ctrl.usfp_src").write(sfpmuxsel)
                echo("upstream sfp {} enabled".format(sfpmuxsel))

                amc_in_bit = 0x1 << (amcmuxsel-1)
                lDevice.getNode("switch.csr.ctrl.amc_in").write(amc_in_bit)
                echo("downstream amc {} enabled".format(amcmuxsel))

                lDevice.dispatch()

            if lBoardType in [ kBoardPC059, kBoardFIB ]:
                lIO.switch_sfp_mux_channel(sfpmuxsel)
                secho("Active sfp mux " + hex(sfpmuxsel), fg='cyan')
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
        echo(lIO.get_hardware_info())    
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
        mux_fib = lIO.read_active_sfp_mux_channel()
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
        echo(lDevice.getNode('io').get_hardware_info())
        if sfp_id is not None:
            echo(lIO.get_sfp_status(sfp_id))
        else:
            if lBoardType in [kBoardFMC , kBoardTLU]:
                echo(lIO.get_sfp_status(0))
            elif lBoardType in [ kBoardPC059, kBoardFIB, kBoardMIB ]:
                # PC059 sfp id 0 is upstream sfp
                if lBoardType == kBoardPC059:
                    lSFPIDRange = 9
                elif lBoardType == kBoardFIB:
                    lSFPIDRange = 8
                elif lBoardType == kBoardMIB:
                    lSFPIDRange = 3
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
        echo(lDevice.getNode('io').get_hardware_info())
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
