from __future__ import print_function

import click
import time
import collections

import pdt.cli.toolbox as toolbox
import pdt.common.definitions as defs

from factory import ShellFactory
from click import echo, style, secho
from pdt.core import SI534xSlave, I2CExpanderSlave, DACSlave

from pdt.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from pdt.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from pdt.common.definitions import kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout

from os.path import join, expandvars, basename

# -----------------------------------------------------------------------------
class BoardShell(object):
    """docstring for BoardShell"""

    # ------------------------------------------------------------------------------
    def freq(self):
        lDevice = self.mDevice
        lBoardType = self.boardType

        secho("PLL Clock frequency measurement:", fg='cyan')
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
    def status(self):
        def decRng( word, ibit, nbits=1):
            return (word >> ibit) & ((1<<nbits)-1)

        lDevice = self.mDevice
        lIO = lDevice.getNode('io')

        echo()
        echo( "--- " + style("IO status", fg='cyan') + " ---")
        lCsrStat = toolbox.readSubNodes(lIO.getNode('csr.stat'))
        toolbox.printRegTable(lCsrStat, False)
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def clkstatus(self, verbose=False):
        def decRng( word, ibit, nbits=1):
            return (word >> ibit) & ((1<<nbits)-1)

        lDevice = self.mDevice
        lBoardType = self.boardType
        lIO = lDevice.getNode('io')

        echo()
        self.status()

        echo()
        self.freq()
        echo()

        # Access the clock chip
        if lBoardType in [kBoardPC059, kBoardTLU]:
            lI2CBusNode = lIO.getNode("i2c")
            lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())
        else:
            lSIChip = lIO.getNode('pll_i2c')

        echo("PLL Configuration id: {}".format(style(lSIChip.readConfigID(), fg='cyan')))
        if verbose:
            secho("PLL Information", fg='cyan')
            lVersion = collections.OrderedDict()
            lVersion['Part number'] = lSIChip.readDeviceVersion()
            lVersion['Device grade'] = lSIChip.readClockRegister(0x4)
            lVersion['Device revision'] = lSIChip.readClockRegister(0x5)
            toolbox.printRegTable(lVersion)

        w = lSIChip.readClockRegister(0xc)

        registers = collections.OrderedDict()
        registers['SYSINCAL'] = decRng(w, 0)
        registers['LOSXAXB'] = decRng(w, 1)
        registers['XAXB_ERR'] = decRng(w, 3)
        registers['SMBUS_TIMEOUT'] = decRng(w, 5)

        w = lSIChip.readClockRegister(0xd)

        registers['LOS'] = decRng(w, 0, 4)
        registers['OOF'] = decRng(w, 4, 4)

        w = lSIChip.readClockRegister(0xe)

        registers['LOL'] = decRng(w, 1)
        registers['HOLD'] = decRng(w, 5)

        w = lSIChip.readClockRegister(0xf)
        registers['CAL_PLL'] = decRng(w, 5)

        w = lSIChip.readClockRegister(0x11)
        registers['SYSINCAL_FLG'] = decRng(w, 0)
        registers['LOSXAXB_FLG'] = decRng(w, 1)
        registers['XAXB_ERR_FLG'] = decRng(w, 3)
        registers['SMBUS_TIMEOUT_FLG'] = decRng(w, 5)

        w = lSIChip.readClockRegister(0x12)
        registers['OOF (sticky)'] = decRng(w, 4, 4)

        secho("PLL Status", fg='cyan')
        toolbox.printRegTable(registers)
# ------------------------------------------------------------------------------

kFMCRev1 = 1
kFMCRev2 = 2
kPC059Rev1 = 3
kPC059FanoutHDMI = 4
kPC059FanoutSFP = 5
kTLURev1 = 6

# kClockConfigMap = {
#     kFMCRev1: "SI5344/PDTS0000.txt",
#     kFMCRev2: "SI5344/PDTS0003.txt",
#     kPC059Rev1: "SI5345/PDTS0005.txt",
#     kPC059FanoutHDMI: "devel/PDTS_PC059_FANOUT.txt",
#     kPC059FanoutSFP: "wr/FANOUT_PLL_WIDEBW_SFPIN.txt",
#     kTLURev1: "wr/TLU_EXTCLK_10MHZ_NOZDM.txt"
# }

# kUIDRevisionMap = {
#     0xd880395e720b: kFMCRev1,
#     0xd880395e501a: kFMCRev1,
#     0xd880395e50b8: kFMCRev1,
#     0xd880395e501b: kFMCRev1,
#     0xd880395e7201: kFMCRev1,
#     0xd880395e4fcc: kFMCRev1,
#     0xd880395e5069: kFMCRev1,
#     0xd880395e7206: kFMCRev1,
#     0xd880395e1c86: kFMCRev2,
#     0xd880395e2630: kFMCRev2,
#     0xd880395e262b: kFMCRev2,
#     0xd880395e2b38: kFMCRev2,
#     0xd880395e1a6a: kFMCRev2,
#     0xd880395e36ae: kFMCRev2,
#     0xd880395e2b2e: kFMCRev2,
#     0xd880395e2b33: kFMCRev2,
#     0xd880395e1c81: kFMCRev2,
#     0xd88039d980cf: kPC059Rev1,
#     0xd88039d98adf: kPC059Rev1,
#     0xd88039d92491: kPC059Rev1,
#     0xd88039d9248e: kPC059Rev1,
#     0xd88039d98ae9: kPC059Rev1,
#     0xd88039d92498: kPC059Rev1,
# }


# ------------------------------------------------------------------------------
class FMCShell(BoardShell):
    """docstring for PC059Shell"""

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
    }

    kClockConfigMap = {
        kFMCRev1: "SI5344/PDTS0000.txt",
        kFMCRev2: "SI5344/PDTS0003.txt",
    }

    def reset(self, soft, forcepllcfg):
        '''
        Perform a hard reset on the timing master, including

        \b
        - ipbus registers
        - i2c buses
        - pll and pll configuration

        \b
        Fanout mode:
        0 = local master
        1 = sfp
        '''

        echo('Resetting ' + click.style(self.mDevice.id(), fg='blue'))

        lDevice = self.mDevice
        lBoardType = self.boardType
        lCarrierType = self.carrierType
        lDesignType = self.designType

        lIO = lDevice.getNode('io')

        # Global soft reset
        lIO.getNode('csr.ctrl.soft_rst').write(0x1)
        lDevice.dispatch()

        time.sleep(0.1)
            
        # PLL and I@C reset 
        lIO.getNode('csr.ctrl.pll_rst').write(0x1)
        lDevice.dispatch()

        lIO.getNode('csr.ctrl.pll_rst').write(0x0)
        lDevice.dispatch()


        # Detect the on-board eprom and read the board UID
        lUID = lIO.getNode('uid_i2c')

        echo('UID I2C Slaves')
        for lSlave in lUID.getSlaves():
            echo("  {}: {}".format(lSlave, hex(lUID.getSlaveAddress(lSlave))))

        if lCarrierType == kCarrierEnclustraA35:

            try:
                # Wake up the switch
                lUID.getSlave('AX3_Switch').writeI2C(0x01, 0x7f)
            except RuntimeError:
                pass

            x = lUID.getSlave('AX3_Switch').readI2C(0x01)
            echo("I2C enable lines: {}".format(x))


        lPROMSlave = 'FMC_UID_PROM'
        lValues = lUID.getSlave(lPROMSlave).readI2CArray(0xfa, 6)
        lUniqueID = 0x0
        for lVal in lValues:
            lUniqueID = ( lUniqueID << 8 ) | lVal
        echo("Timing Board PROM UID: "+style(hex(lUniqueID), fg="blue"))

        if lBoardType != kBoardTLU:
            # Ensure that the board is known to the revision DB
            try:
                lRevision = self.kUIDRevisionMap[lUniqueID]
            except KeyError:
                raise click.ClickException("No revision associated to UID "+hex(lUniqueID))

        # Access the clock chip
        lSIChip = lIO.getNode('pll_i2c')
        lSIVersion = lSIChip.readDeviceVersion()
        echo("PLL version : "+style(hex(lSIVersion), fg='blue'))

        # Ensure that the board revision has a registered clock config
        if forcepllcfg is not None:
            lFullClockConfigPath = forcepllcfg
            echo("Using PLL Clock configuration file: "+style(basename(lFullClockConfigPath), fg='green') )

        else:
            if lDesignType == kDesingFanout and fanout in [0]:
                secho("Overriding clock config - fanout mode", fg='green')
                lClockConfigPath = self.kClockConfigMap[kPC059FanoutHDMI if fanout == 1 else kPC059FanoutSFP]
            else:
                try:
                    lClockConfigPath = self.kClockConfigMap[lRevision]    
                except KeyError:
                    raise ClickException("Board revision " << lRevision << " has no associated clock configuration")


            echo("PLL Clock configuration file: "+style(lClockConfigPath, fg='green') )

            # Configure the clock chip
            lFullClockConfigPath = expandvars(join('${PDT_TESTS}/etc/clock', lClockConfigPath))

        lSIChip.configure(lFullClockConfigPath)
        echo("SI354x configuration id: {}".format(style(lSIChip.readConfigID(), fg='green')))


        if lDesignType == kDesingFanout:
            lDevice.getNode('switch.csr.ctrl.master_src').write(fanout)
            lIO.getNode('csr.ctrl.mux').write(0)
            lDevice.dispatch()

        self.clkstatus()

        lDevice.getNode("io.csr.ctrl.sfp_tx_dis").write(0)
        lDevice.dispatch()

# ------------------------------------------------------------------------------

ShellFactory.registerBoard(kBoardFMC, FMCShell)

# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
class PC059Shell(BoardShell):

    kUIDRevisionMap = {
        0xd88039d980cf: kPC059Rev1,
        0xd88039d98adf: kPC059Rev1,
        0xd88039d92491: kPC059Rev1,
        0xd88039d9248e: kPC059Rev1,
        0xd88039d98ae9: kPC059Rev1,
        0xd88039d92498: kPC059Rev1,
    }

    kClockConfigMap = {
        kPC059Rev1: "SI5345/PDTS0005.txt",
        kPC059FanoutHDMI: "devel/PDTS_PC059_FANOUT.txt",
        kPC059FanoutSFP: "wr/FANOUT_PLL_WIDEBW_SFPIN.txt",
    }

    def reset(self, soft, fanout, forcepllcfg):
        '''
        Perform a hard reset on the timing master, including

        \b
        - ipbus registers
        - i2c buses
        - pll and pll configuration

        \b
        Fanout mode:
        0 = local master
        1 = sfp
        '''


        lDevice = self.mDevice
        lBoardType = self.boardType
        lCarrierType = self.carrierType
        lDesignType = self.designType

        echo('Resetting ' + click.style(lDevice.id(), fg='blue'))

        lIO = lDevice.getNode('io')

        if ( fanout ):
            secho("Fanout mode enabled", fg='green')

        # Global soft reset
        lIO.getNode('csr.ctrl.soft_rst').write(0x1)
        lDevice.dispatch()


        if not soft:
            
            time.sleep(1)
            
            # PLL and I@C reset 
            lIO.getNode('csr.ctrl.pll_rst').write(0x1)
            lIO.getNode('csr.ctrl.rst_i2c').write(0x1)
            lIO.getNode('csr.ctrl.rst_i2cmux').write(0x1)
            lDevice.dispatch()

            lIO.getNode('csr.ctrl.pll_rst').write(0x0)
            lIO.getNode('csr.ctrl.rst_i2c').write(0x0)
            lIO.getNode('csr.ctrl.rst_i2cmux').write(0x0)
            
            lDevice.dispatch()

            # Detect the on-board eprom and read the board UID
            lUID = lIO.getNode('i2c')

            echo('UID I2C Slaves')
            for lSlave in lUID.getSlaves():
                echo("  {}: {}".format(lSlave, hex(lUID.getSlaveAddress(lSlave))))

            try:
                # Wake up the switch
                lUID.getSlave('AX3_Switch').writeI2C(0x01, 0x7f)
            except RuntimeError:
                pass

            x = lUID.getSlave('AX3_Switch').readI2C(0x01)
            echo("I2C enable lines: {}".format(x))

            lPROMSlave = 'FMC_UID_PROM'
            lValues = lUID.getSlave(lPROMSlave).readI2CArray(0xfa, 6)
            lUniqueID = 0x0
            for lVal in lValues:
                lUniqueID = ( lUniqueID << 8 ) | lVal
            echo("Timing Board PROM UID: "+style(hex(lUniqueID), fg="blue"))

            # Access the clock chip
            lI2CBusNode = lDevice.getNode("io.i2c")
            lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())
            lSIVersion = lSIChip.readDeviceVersion()
            echo("PLL version : "+style(hex(lSIVersion), fg='blue'))

            # Ensure that the board is known to the revision DB
            try:
                lRevision = self.kUIDRevisionMap[lUniqueID]
            except KeyError:
                raise click.ClickException("No revision associated to UID "+hex(lUniqueID))

            # Ensure that the board revision has a registered clock config
            if forcepllcfg is not None:
                lFullClockConfigPath = forcepllcfg
                echo("Using PLL Clock configuration file: "+style(basename(lFullClockConfigPath), fg='green') )

            else:
                if lDesignType == kDesingFanout and fanout in [0]:
                    secho("Overriding clock config - fanout mode", fg='green')
                    lClockConfigPath = self.kClockConfigMap[kPC059FanoutHDMI if fanout == 1 else kPC059FanoutSFP]
                else:
                    try:
                        lClockConfigPath = self.kClockConfigMap[lRevision]    
                    except KeyError:
                        raise ClickException("Board revision " << lRevision << " has no associated clock configuration")


                echo("PLL Clock configuration file: "+style(lClockConfigPath, fg='green') )

                # Configure the clock chip
                lFullClockConfigPath = expandvars(join('${PDT_TESTS}/etc/clock', lClockConfigPath))

            lSIChip.configure(lFullClockConfigPath)
            echo("SI3545 configuration id: {}".format(style(lSIChip.readConfigID(), fg='green')))

            self.clkstatus()

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

            lIO.getNode('csr.ctrl.rst_lock_mon').write(0x1)
            lIO.getNode('csr.ctrl.rst_lock_mon').write(0x0)
            lIO.getClient().dispatch()

            if lDesignType == kDesingFanout:
                lDevice.getNode('switch.csr.ctrl.master_src').write(fanout)
                lIO.getNode('csr.ctrl.mux').write(0)
                lDevice.dispatch()

        echo()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
    def selectMux(self, mux):
        lDevice = self.mDevice
        
        lDevice.getNode('io.csr.ctrl.mux').write(mux)
        lDevice.dispatch()
        echo('SFP input mux set to {}'.format(mux))
# ------------------------------------------------------------------------------

ShellFactory.registerBoard(kBoardPC059, PC059Shell)

# ------------------------------------------------------------------------------
                

# ------------------------------------------------------------------------------
class TLUShell(BoardShell):
    """docstring for PC059Shell"""
    kUIDRevisionMap = {
        0x5410ecbb9426: kTLURev1,
    }

    kClockConfigMap = {
        kTLURev1: "wr/TLU_EXTCLK_10MHZ_NOZDM.txt"
    }

    def reset(ctx, obj, soft, fanout, forcepllcfg):
        '''
        Perform a hard reset on the timing master, including

        \b
        - ipbus registers
        - i2c buses
        - pll and pll configuration

        \b
        Fanout mode:
        0 = local master
        1 = sfp
        '''

        lDevice = obj.device
        lBoardType = obj.boardType
        lCarrierType = obj.carrierType
        lDesignType = obj.designType

        echo('Resetting ' + click.style(lDevice.id(), fg='blue'))

        lIO = lDevice.getNode('io')

        # Global soft reset
        lIO.getNode('csr.ctrl.soft_rst').write(0x1)
        lDevice.dispatch()

        if not (soft or lBoardType == kBoardSim):
            
            time.sleep(0.1)
            
            # PLL and I@C reset 
            lIO.getNode('csr.ctrl.pll_rst').write(0x1)
            lIO.getNode('csr.ctrl.rst_i2c').write(0x1)
            lDevice.dispatch()

            lIO.getNode('csr.ctrl.pll_rst').write(0x0)
            lIO.getNode('csr.ctrl.rst_i2c').write(0x0)
            lDevice.dispatch()

            # Detect the on-board eprom and read the board UID
            lUID = lIO.getNode('i2c')

            echo('UID I2C Slaves')
            for lSlave in lUID.getSlaves():
                echo("  {}: {}".format(lSlave, hex(lUID.getSlaveAddress(lSlave))))

            try:
                # Wake up the switch
                lUID.getSlave('AX3_Switch').writeI2C(0x01, 0x7f)
            except RuntimeError:
                pass

            x = lUID.getSlave('AX3_Switch').readI2C(0x01)
            echo("I2C enable lines: {}".format(x))


            lPROMSlave = 'UID_PROM'
            lValues = lUID.getSlave(lPROMSlave).readI2CArray(0xfa, 6)
            lUniqueID = 0x0
            for lVal in lValues:
                lUniqueID = ( lUniqueID << 8 ) | lVal
            echo("Timing Board PROM UID: "+style(hex(lUniqueID), fg="blue"))

            # Ensure that the board is known to the revision DB
            try:
                lRevision = kUIDRevisionMap[lUniqueID]
            except KeyError:
                raise click.ClickException("No revision associated to UID "+hex(lUniqueID))

            # Access the clock chip
            lI2CBusNode = lDevice.getNode("io.i2c")
            lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())
            lSIVersion = lSIChip.readDeviceVersion()
            echo("PLL version : "+style(hex(lSIVersion), fg='blue'))

            # Ensure that the board revision has a registered clock config
            if forcepllcfg is not None:
                lFullClockConfigPath = forcepllcfg
                echo("Using PLL Clock configuration file: "+style(basename(lFullClockConfigPath), fg='green') )
            else:
                try:
                    lClockConfigPath = kClockConfigMap[lRevision]    
                except KeyError:
                    raise ClickException("Board revision " << lRevision << " has no associated clock configuration")

                echo("PLL Clock configuration file: "+style(lClockConfigPath, fg='green') )

                # Configure the clock chip
                lFullClockConfigPath = expandvars(join('${PDT_TESTS}/etc/clock', lClockConfigPath))

            lSIChip.configure(lFullClockConfigPath)
            echo("SI3545 configuration id: {}".format(style(lSIChip.readConfigID(), fg='green')))

            self.clkstatus()

            lIC6 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('Expander1').getI2CAddress())
            lIC7 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('Expander2').getI2CAddress())

            # Bank 0
            lIC6.setInversion(0, 0x00)
            lIC6.setIO(0, 0x00)
            lIC6.setOutputs(0, 0x00)

            # Bank 1
            lIC6.setInversion(1, 0x00)
            lIC6.setIO(1, 0x00)
            lIC6.setOutputs(1, 0x88)


            # Bank 0
            lIC7.setInversion(0, 0x00)
            lIC7.setIO(0, 0x00)
            lIC7.setOutputs(0, 0xf0)

            # Bank 1
            lIC7.setInversion(1, 0x00)
            lIC7.setIO(1, 0x00)
            lIC7.setOutputs(1, 0xf0)

            # Tweak the PLL swing
            lI2CBusNode = lDevice.getNode("io.i2c")
            lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())

            lSIChip.writeI2CArray(0x113, [0x9, 0x33])


            lDAC1 = DACSlave(lI2CBusNode, lI2CBusNode.getSlave('DAC1').getI2CAddress())
            lDAC2 = DACSlave(lI2CBusNode, lI2CBusNode.getSlave('DAC2').getI2CAddress())

            # BI signals are NIM
            lBISignalThreshold = 0x589D

            lDAC1.setInteralRef(False);
            lDAC2.setInteralRef(False);
            lDAC1.setDAC(7, lBISignalThreshold);
            lDAC2.setDAC(7, lBISignalThreshold);

            secho("DAC1 and DAC2 set to " + hex(lBISignalThreshold), fg='cyan')

            lIO.getNode('csr.ctrl.rst_lock_mon').write(0x1)
            lIO.getNode('csr.ctrl.rst_lock_mon').write(0x0)
            lIO.getClient().dispatch()


        echo()
# ------------------------------------------------------------------------------

ShellFactory.registerBoard(kBoardTLU, TLUShell)

# ------------------------------------------------------------------------------
