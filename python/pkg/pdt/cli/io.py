from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import math
import pdt

import pdt.cli.toolbox as toolbox
import pdt.common.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars, basename
from pdt.core import SI534xSlave, I2CExpanderSlave, DACSlave

from pdt.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU, kBoardFIB
from pdt.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed, kCarrierAFC
from pdt.common.definitions import kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout
from pdt.common.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap
from pdt.common.definitions import kLibrarySupportedBoards


kFMCRev1 = 1
kFMCRev2 = 2
kFMCRev3 = 7
kPC059Rev1 = 3
kPC059FanoutHDMI = 4
kPC059FanoutSFP = 5
kTLURev1 = 6

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
@click.option('--sfp-mux-sel', 'sfpmuxsel', type=toolbox.IntRange(0x0,0x7), default=0, help='Configures the sfp cdr mux on the fib')
@click.option('--force-pll-cfg', 'forcepllcfg', type=click.Path(exists=True))
@click.pass_obj
@click.pass_context
def reset(ctx, obj, soft, fanout, forcepllcfg, sfpmuxsel):
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
            lIO.softReset()
            return
        
        if lDesignType == kDesingFanout:
            
            if fanout == 0:
                secho("Fanout mode enabled", fg='green')
            
            lIO.reset(fanout, lPLLConfigFilePath)
            lDevice.getNode('switch.csr.ctrl.master_src').write(fanout)
            lDevice.dispatch()
        else:
            lIO.reset(lPLLConfigFilePath)            
    
        ctx.invoke(clkstatus)
    
    
       
    elif lBoardType == kBoardFIB:
            echo("fib reset")

            lIO.reset()

            return
            
            #reset expanders, reset of pll is done later
            lIO.getNode('csr.ctrl.rstb_i2c').write(0x1)
            lDevice.dispatch()
            lIO.getNode('csr.ctrl.rstb_i2c').write(0x0)
            lDevice.dispatch()

            lUID = lIO.getNode('i2c')

            echo('UID I2C Slaves')
            for lSlave in lUID.getSlaves():
                echo("  {}: {}".format(lSlave, hex(lUID.getSlaveAddress(lSlave))))

            try:
                # Wake up the switch
                lUID.getSlave('AX3_Switch').writeI2C(0x01, 0x7f)
                print('AX3_Switch working')
            except RuntimeError:
                passlPROMSlave = 'AFC_FMC_UID_PROM'
            x = lUID.getSlave('AX3_Switch').readI2C(0x01)
            echo("I2C enable lines: {}".format(x))

            #updae prom address for real fib on enclustra, at the momment it looks at old fmc address

            lPROMSlave = 'AFC_FMC_UID_PROM'

            lValues = lUID.getSlave(lPROMSlave).readI2CArray(0xfa, 6)
            lUniqueID = 0x0
            for lVal in lValues:
                lUniqueID = ( lUniqueID << 8 ) | lVal
            echo("Timing Board PROM UID: "+style(hex(lUniqueID), fg="blue"))

            #configuration of FIB expanders 

                       
            lI2CBusNode = lDevice.getNode("io.i2c")
           
            lExpander1 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('Expander1').getI2CAddress())
            lExpander2 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('Expander2').getI2CAddress())
            
            #confugre sfp los receiver bank
            lExpander2.setInversion(1, 0x00)
            lExpander2.setIO(1, 0xff)
            
            #configure clk and cdr signals
            lExpander2.setInversion(0, 0x00)  #bank 1, no inversion 
            lExpander2.setIO(0, 0xfe)         #bank 0, 0001 1110
            lExpander2.setOutputs(0, 0x01);


            # sfp tx enable
            lExpander1.setInversion(0, 0x00)
            lExpander1.setIO(0, 0x00)
            lExpander1.setOutputs(0, 0x00)

            #confugre sfp fault receiver bank
            lExpander1.setInversion(1, 0x00)
            lExpander1.setIO(1, 0xff)


            # Access the clock chip
            #lI2CBusNode = lDevice.getNode("io.i2c")
            #lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())

            echo("pll reset")
            lExpander2.setOutputs(0, 0x00);
            lExpander2.setOutputs(0, 0x01);

            lIO.getNode('csr.ctrl.inmux').write(sfpmuxsel)
            lIO.getClient().dispatch()
            secho("Active sfp mux " + hex(sfpmuxsel), fg='cyan')
            
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
        echo(lIO.getClockFrequenciesTable())
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
        echo(lIO.getHardwareInfo())    
        echo(lIO.getStatus())
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
        echo(lIO.getPLLStatus())
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
        lIO.configureDAC(0,value)
        lIO.configureDAC(1,value)
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
        echo(lDevice.getNode('io').getHardwareInfo())
        if sfp_id is not None:
            echo(lIO.getSFPStatus(sfp_id))
        else:
            if lBoardType == kBoardFMC or lBoardType == kBoardTLU:
                echo(lIO.getSFPStatus(0))
            elif lBoardType in [ kBoardPC059, kBoardFIB]:
                # PC059 sfp id 0 is upstream sfp
                lSFPIDRange = 9 if lBoardType == kBoardPC059 else 8
                for i in range(lSFPIDRange):
                    try:
                        echo(lIO.getSFPStatus(i))
                        #echo()
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
@click.option('--hard', '-h', is_flag=True, default=False, help='Control SFP tx disable pin (FIB only).')
def switchsfptx(ctx, obj, sfp_id, on, hard):
    '''
    Toggle SFP tx disable reg (if supported)
    '''

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lCarrierType = obj.mCarrierType
    lDesignType = obj.mDesignType

    lIO = lDevice.getNode('io') 

    if lBoardType in kLibrarySupportedBoards:
        echo(lDevice.getNode('io').getHardwareInfo())

        if sfp_id is not None:
            if not hard:
                lIO.switchSFPSoftTxControl(sfp_id, on)
            else:
                if lBoardType == kBoardFIB:
                    lIO.switchSFPTx(sfp_id, on)
                else:
                    secho("SFP tx disable pin control only supported for FIB", fg='yellow')
            echo(lIO.getSFPStatus(sfp_id))
        else:
            # only one SFP on these boards, no need for sfp id to be supplied
            if lBoardType in [kBoardFMC, kBoardTLU]:
                if not hard:
                    lIO.switchSFPSoftTxControl(0, on)
                else:
                    secho("SFP tx disable pin control only supported for FIB", fg='yellow')
                echo(lIO.getSFPStatus(0))
            else:
                secho("Multiple SFPs on board {}. Please supply id of SFP to control".format(kBoardNamelMap[lBoardType]), fg='yellow')        
    else:
        secho("Board {} not supported by timing library".format(lBoardType), fg='yellow')
        # do sfp switch here
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@io.command('sfp-flags', short_help="Read sfp status flags over I2C (FIB only)")
@click.pass_obj
@click.pass_context
@click.option('--sfp-id', 'sfp_id', required=False, type=click.IntRange(0, 7), help='SFP id to query.')
def sfpflags(ctx, obj, sfp_id):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lCarrierType = obj.mCarrierType
    lDesignType = obj.mDesignType
    
    lIO = lDevice.getNode('io')
    lUID = lIO.getNode('i2c')

    if lBoardType == kBoardFIB:

        lSFPLOSFlags = lIO.readSFPLOSFlags()
        lSFPFaultFlags = lIO.readSFPFaultFlags()

        secho("SFP LOS flags: {}".format('{0:08b}'.format(lSFPLOSFlags)))
        secho("SFP Fault flags: {}".format('{0:08b}'.format(lSFPFaultFlags)))

        if sfp_id is not None:
            lSFPLOSFlag = lSFPLOSFlags & (1 << sfp_id)
            secho("SFP {} LOS flag: {}".format(sfp_id,hex(lSFPLOSFlag)))

            lSFPFaultFlag = lSFPFaultFlags & (1 << sfp_id)
            secho("SFP {} Fault flag: {}".format(sfp_id,hex(lSFPFaultFlag)))
    else:
        secho("I2C SFP flag reading for FIB only")
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@io.command('debug')
@click.pass_obj
@click.pass_context
def debug(ctx, obj):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lCarrierType = obj.mCarrierType
    lDesignType = obj.mDesignType

    lIO = lDevice.getNode('io')

    # Global soft reset
    #lIO.getNode('csr.ctrl.soft_rst').write(0x1)
    #lIO.getNode('csr.ctrl.soft_rst').write(0x0)
    #lDevice.dispatch()
    
    # Control I2C reset line (inverted in fw)
    #lIO.getNode('csr.ctrl.rstb_i2c').write(0x0)
    #lDevice.dispatch()

    # Get the main I2C bus master
    lI2CBusNode = lDevice.getNode("io.i2c")
    
    # Do an I2C bus address scan
    i2cdevices = lI2CBusNode.scan()
    # Print the list of addresses which responded
    print ('[{}]'.format(', '.join(hex(x) for x in i2cdevices)))

    # Attempt an I2C transaction with a particular address
    # True if transaction successful, False if not
    #print(lI2CBusNode.ping(0x74))
    #print(lI2CBusNode.ping(0x50))
# ------------------------------------------------------------------------------

