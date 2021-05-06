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

from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignChronos, kDesignBoreas
from timing.common.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap
from timing.common.definitions import kLibrarySupportedBoards


kFMCRev1 = 1
kFMCRev2 = 2
kFMCRev3 = 7
kPC059Rev1 = 3
kPC059FanoutHDMI = 4
kPC059FanoutSFP = 5
kTLURev1 = 6
kFMCRev3 = 7

kClockConfigMap = {
    kFMCRev1: "SI5344/PDTS0000.txt",
    kFMCRev2: "SI5344/PDTS0003.txt",
    kFMCRev3: "SI5394/PDTS0003.txt",
    kPC059Rev1: "SI5345/PDTS0005.txt",
    kPC059FanoutHDMI: "devel/PDTS_PC059_FANOUT.txt",
    kPC059FanoutSFP: "wr/FANOUT_PLL_WIDEBW_SFPIN.txt",
    kTLURev1: "wr/TLU_EXTCLK_10MHZ_NOZDM.txt"
}

kUIDRevisionMap = {
    0xd880395e720b: kFMCRev1,
    0xd880395e501a: kFMCRev1,
    0xd880395e50b8: kFMCRev1,
    0xd880395e501b: kFMCRev1,
    0xd880395e7201: kFMCRev1,
    0xd880395e4fcc: kFMCRev1,
    0xd880395e5069: kFMCRev1,
    0xd880395e7206: kFMCRev1,
    0xd880395e1c86: kFMCRev2,
    0xd880395e2630: kFMCRev2,
    0xd880395e262b: kFMCRev2,
    0xd880395e2b38: kFMCRev2,
    0xd880395e1a6a: kFMCRev2,
    0xd880395e36ae: kFMCRev2,
    0xd880395e2b2e: kFMCRev2,
    0xd880395e2b33: kFMCRev2,
    0xd880395e1c81: kFMCRev2,
    0x049162b67ce6: kFMCRev3,
    0x049162b62947: kFMCRev3,
    0x049162b67cdf: kFMCRev3,
    0x49162b62050 : kFMCRev3,
    0x49162b62951 : kFMCRev3,
    0x49162b675e3 : kFMCRev3,
    0xd88039d980cf: kPC059Rev1,
    0xd88039d98adf: kPC059Rev1,
    0xd88039d92491: kPC059Rev1,
    0xd88039d9248e: kPC059Rev1,
    0xd88039d98ae9: kPC059Rev1,
    0xd88039d92498: kPC059Rev1,
    0x5410ecbba408: kTLURev1,
    0x5410ecbb9426: kTLURev1,
    #0x801f12f5ce48: kFIBRev1},
    #0x801f12f5e9ae: kFIBRev1,
    0x49162b65025:  kFMCRev3,
    0x49162b62948:  kFMCRev3,
}

# ------------------------------------------------------------------------------
#    __  ___         __         
#   /  |/  /__ ____ / /____ ____
#  / /|_/ / _ `(_-</ __/ -_) __/
# /_/  /_/\_,_/___/\__/\__/_/   
                        

@click.group('io', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, autocompletion=toolbox.completeDevices)
def io(obj, device):
    '''
    Timing master commands.

    DEVICE: uhal device identifier
    '''
    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)
        
    echo('Created device ' + click.style(lDevice.id(), fg='blue'))
    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lDevice.dispatch()


    echo("Design '{}' on board '{}' on carrier '{}'".format(
        style(kDesignNameMap[lBoardInfo['design_type'].value()], fg='blue'),
        style(kBoardNamelMap[lBoardInfo['board_type'].value()], fg='blue'),
        style(kCarrierNamelMap[lBoardInfo['carrier_type'].value()], fg='blue')
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
@click.option('--force-pll-cfg', 'forcepllcfg', type=click.Path(exists=True))
@click.pass_obj
@click.pass_context
def reset(ctx, obj, soft, fanout, forcepllcfg):
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
            
            lIO.reset(fanout, lPLLConfigFilePath)
            lDevice.getNode('switch.csr.ctrl.master_src').write(fanout)
            lDevice.dispatch()
        else:
            lIO.reset(lPLLConfigFilePath)            
    
        ctx.invoke(clkstatus)

    else:
        secho("Board {} not supported by timing library".format(lBoardType), fg='yellow')
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
    lIO = lDevice.getNode('io')
    lBoardType = obj.mBoardType
    
    ctx.invoke(status)

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
            if lBoardType == kBoardFMC or lBoardType == kBoardTLU:
                echo(lIO.get_sfp_status(0))
            elif ( lBoardType == kBoardPC059 ):
                for i in range(9):
                    try:
                        echo(lIO.get_sfp_status(i))
                        echo()
                    except:
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
