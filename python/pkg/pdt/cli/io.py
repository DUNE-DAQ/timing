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
import pdt.cli.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars, basename
from pdt.core import SI5344Slave, SI534xSlave, I2CExpanderSlave

from pdt.cli.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from pdt.cli.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from pdt.cli.definitions import kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout
from pdt.cli.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap


kFMCRev1 = 1
kFMCRev2 = 2
kPC059Rev1 = 3
kPC059FanoutHDMI = 4
kPC059FanoutSFP = 5
kTLURev1 = 6

kClockConfigMap = {
    kFMCRev1: "SI5344/PDTS0000.txt",
    kFMCRev2: "SI5344/PDTS0003.txt",
    kPC059Rev1: "SI5345/PDTS0005.txt",
    kPC059FanoutHDMI: "devel/PDTS_PC059_FANOUT.txt",
    kPC059FanoutSFP: "devel/PDTS_PC059_FANOUT_SFP_IN.txt",
    # kTLURev1: "devel/PDTS_TLU_MASTER.txt"
    kTLURev1: "devel/PDTS_TLU_MASTER_ONLYLEMOIN.txt"
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
@click.argument('device', callback=toolbox.validate_device)
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
@io.command('reset', short_help="Perform a hard reset of the board io infrastructure.")
@click.option('--soft', '-s', is_flag=True, default=False, help='Soft reset i.e. skip the clock chip configuration.')
@click.option('--fanout-mode', 'fanout', type=click.IntRange(0, 3), default=0, help='Configures the board in fanout mode (pc059 only)')
@click.option('--force-pll-cfg', 'forcepllcfg', type=click.Path(exists=True))
@click.pass_obj
@click.pass_context
def reset(ctx, obj, soft, fanout, forcepllcfg):
    '''
    Perform a hard reset on the board io including

    \b
    - ipbus registers
    - i2c buses
    - pll and pll configuration

    \b
    Fanout mode:
    0 = local master
    1 = hdmi
    2 = sfp
    '''

    echo('Resetting ' + click.style(obj.mDevice.id(), fg='blue'))

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lCarrierType = obj.mCarrierType

    if ( lBoardType == kBoardPC059 and fanout ):
        secho("Fanout mode enabled", fg='green')

    # Global soft reset
    lDevice.getNode('io.csr.ctrl.soft_rst').write(0x1)
    lDevice.dispatch()


    if not (soft or lBoardType == kBoardSim):
        
        time.sleep(1)
        
        # PLL and I@C reset 
        lDevice.getNode('io.csr.ctrl.pll_rst').write(0x1)
        if lBoardType == kBoardPC059:
            lDevice.getNode('io.csr.ctrl.rst_i2c').write(0x1)
            lDevice.getNode('io.csr.ctrl.rst_i2cmux').write(0x1)


        elif lBoardType == kBoardTLU:
            lDevice.getNode('io.csr.ctrl.rst_i2c').write(0x1)

        lDevice.dispatch()

        lDevice.getNode('io.csr.ctrl.pll_rst').write(0x0)
        if lBoardType == kBoardPC059:
            lDevice.getNode('io.csr.ctrl.rst_i2c').write(0x0)
            lDevice.getNode('io.csr.ctrl.rst_i2cmux').write(0x0)
        
        elif lBoardType == kBoardTLU:
            lDevice.getNode('io.csr.ctrl.rst_i2c').write(0x0)

        lDevice.dispatch()


        # Detect the on-board eprom and read the board UID
        if lBoardType in [kBoardPC059, kBoardTLU]:
            lUID = lDevice.getNode('io.i2c')
        else:
            lUID = lDevice.getNode('io.uid_i2c')

        echo('UID I2C Slaves')
        for lSlave in lUID.getSlaves():
            echo("  {}: {}".format(lSlave, hex(lUID.getSlaveAddress(lSlave))))

        if (
            lBoardType == kBoardTLU or
            lBoardType == kBoardPC059 or
            (lBoardType == kBoardFMC and lCarrierType == kCarrierEnclustraA35)
            ):
            lUID.getSlave('AX3_Switch').writeI2C(0x01, 0x7f)
            x = lUID.getSlave('AX3_Switch').readI2C(0x01)
            echo("I2C enable lines: {}".format(x))
        elif lCarrierType == kCarrierKC705:
            lUID.getSlave('KC705_Switch').writeI2CPrimitive([0x10])
            # x = lUID.getSlave('KC705_Switch').readI2CPrimitive(1)
            echo("KC705 I2C switch enabled (hopefully)".format())
        else:
            click.ClickException("Unknown board kind {}".format(lBoardType))


        # 
        # If not a TLU, read the unique ID from the prom 
        # if lBoardType != kBoardTLU:

        lPROMSlave = 'UID_PROM' if lBoardType == kBoardTLU else 'FMC_UID_PROM'
        lValues = lUID.getSlave(lPROMSlave).readI2CArray(0xfa, 6)
        lUniqueID = 0x0
        for lVal in lValues:
            lUniqueID = ( lUniqueID << 8 ) | lVal
        echo("Timing Board PROM UID: "+style(hex(lUniqueID), fg="blue"))

        if lBoardType != kBoardTLU:
            # Ensure that the board is known to the revision DB
            try:
                lRevision = kUIDRevisionMap[lUniqueID]
            except KeyError:
                raise click.ClickException("No revision associated to UID "+hex(lUniqueID))

        # Access the clock chip
        if lBoardType in [kBoardPC059, kBoardTLU]:
            lI2CBusNode = lDevice.getNode("io.i2c")
            lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())
        else:
            lSIChip = lDevice.getNode('io.pll_i2c')
        lSIVersion = lSIChip.readDeviceVersion()
        echo("PLL version : "+style(hex(lSIVersion), fg='blue'))

        # Ensure that the board revision has a registered clock config
        if forcepllcfg is not None:
            lFullClockConfigPath = forcepllcfg
            echo("Using SI3545 Clock configuration file: "+style(basename(lFullClockConfigPath), fg='green') )

        else:
            if lBoardType == kBoardTLU:
                lClockConfigPath = kClockConfigMap[kTLURev1]
            elif lBoardType == kBoardPC059 and fanout in [1,2]:
                secho("Overriding clock config - fanout mode", fg='green')
                lClockConfigPath = kClockConfigMap[kPC059FanoutHDMI if fanout == 1 else kPC059FanoutSFP]
            else:
                try:
                    lClockConfigPath = kClockConfigMap[lRevision]    
                except KeyError:
                    raise ClickException("Board revision " << lRevision << " has no associated clock configuration")


            echo("SI3545 Clock configuration file: "+style(lClockConfigPath, fg='green') )

            # Configure the clock chip
            lFullClockConfigPath = expandvars(join('${PDT_TESTS}/etc/clock', lClockConfigPath))

        lSIChip.configure(lFullClockConfigPath)
        echo("SI3545 configuration id: {}".format(style(lSIChip.readConfigID(), fg='green')))


        if lBoardType == kBoardPC059:
            lDevice.getNode('io.csr.ctrl.master_src').write(fanout)
            lDevice.getNode('io.csr.ctrl.cdr_edge').write(1)
            lDevice.getNode('io.csr.ctrl.sfp_edge').write(1)
            lDevice.getNode('io.csr.ctrl.hdmi_edge').write(0)
            lDevice.getNode('io.csr.ctrl.usfp_edge').write(1)
            lDevice.getNode('io.csr.ctrl.mux').write(0)
            lDevice.dispatch()

        ctx.invoke(freq)

        if lBoardType == kBoardFMC:
            lDevice.getNode("io.csr.ctrl.sfp_tx_dis").write(0)
            lDevice.dispatch()
        elif lBoardType == kBoardPC059:
            lI2CBusNode = lDevice.getNode("io.i2c")
            lSFPExp = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('SFPExpander').getI2CAddress())

            # Set invert registers to default for both banks
            lSFPExp.setInversion(0, 0x00)
            lSFPExp.setInversion(1, 0x00)

            # BAnk 0 input, bank 1 output
            lSFPExp.setIO(0, 0x00)
            lSFPExp.setIO(1, 0xff)

            # Bank 0 - enable all SFPGs (enable low)
            lSFPExp.setOutputs(0, 0x00)
            secho("SFPs 0-7 enabled", fg='cyan')
        elif lBoardType == kBoardTLU:

            lIC6 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('Expander1').getI2CAddress())
            lIC7 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('Expander2').getI2CAddress())

            # Bank 0
            lIC6.setInversion(0, 0x00)
            lIC6.setIO(0, 0x00)
            lIC6.setOutputs(0, 0x00)

            # Bank 1
            lIC6.setInversion(1, 0x00)
            lIC6.setIO(1, 0x00)
            lIC6.setOutputs(1, 0x80)


            # Bank 0
            lIC7.setInversion(0, 0x00)
            lIC7.setIO(0, 0x00)
            lIC7.setOutputs(0, 0xF0)

            # Bank 1
            lIC7.setInversion(1, 0x00)
            lIC7.setIO(1, 0x00)
            lIC7.setOutputs(1, 0x00)


            # Enable all busy outputs (CTB connection test)
            # lIC6.setOutputs(0, 0x88)
            # lIC6.setOutputs(0, 0x88)
            # lIC7.setOutputs(0, 0xF0)
            # lIC7.setOutputs(0, 0xE0)

        else:
            click.ClickException("Unknown board kind {}".format(lBoardType))

    echo()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@io.command('freq', short_help="Measure some frequencies.")
@click.pass_obj
def freq(obj):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType

    # Measure the generated clock frequency
    freqs = {}
    for i in range(1 if lBoardType == kBoardTLU else 2):
        lDevice.getNode("io.freq.ctrl.chan_sel").write(i)
        lDevice.getNode("io.freq.ctrl.en_crap_mode").write(0)
        lDevice.dispatch()
        time.sleep(2)
        fq = lDevice.getNode("io.freq.freq.count").read()
        fv = lDevice.getNode("io.freq.freq.valid").read()
        lDevice.dispatch()
        freqs[i] = int(fq) * 119.20928 / 1000000 if fv else 'NaN'

    print( "Freq PLL:", freqs[0] )
    if lBoardType != kBoardTLU:
        print( "Freq CDR:", freqs[1] )   
# ------------------------------------------------------------------------------

