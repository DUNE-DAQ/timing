from __future__ import print_function

import click
import time
import collections

import pdt.cli.toolbox as toolbox
import pdt.common.definitions as defs

from factory import ShellFactory, ShellContext
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
    def softReset(self):
        lIO = self.device.getNode('io')

        # Global soft reset
        lIO.getNode('csr.ctrl.soft_rst').write(0x1)
        lIO.getClient().dispatch()
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def enableI2CSwitch(self):
        try:
            # Wake up the switch
            self.getAX3Slave().writeI2C(0x01, 0x7f)
        except RuntimeError:
            pass

        x = self.getAX3Slave().readI2C(0x01)
        return x
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def readUID(self):
        lValues = self.getUIDSlave().readI2CArray(0xfa, 6)
        lUniqueID = 0x0
        for lVal in lValues:
            lUniqueID = ( lUniqueID << 8 ) | lVal
        return lUniqueID
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def freq(self):
        lDevice = self.device
        lBoardType = self.info.boardType

        # secho("PLL Clock frequency measurement:", fg='cyan')
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

        lFreqs = {
            'PLL': freqs[0]
        }
        # print( "Freq PLL:", freqs[0] )
        if lBoardType != kBoardTLU:
            # print( "Freq CDR:", freqs[1] )
            lFreqs['CDR'] = freqs[1]

        return lFreqs

    # ------------------------------------------------------------------------------
    def status(self):
        def decRng( word, ibit, nbits=1):
            return (word >> ibit) & ((1<<nbits)-1)

        lDevice = self.device
        lIO = lDevice.getNode('io')

        # echo()
        # echo( "--- " + style("IO status", fg='cyan') + " ---")
        return toolbox.readSubNodes(lIO.getNode('csr.stat'))
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def pllstatus(self, verbose=False):
        def decRng( word, ibit, nbits=1):
            return (word >> ibit) & ((1<<nbits)-1)

        lDevice = self.device
        lBoardType = self.info.boardType
        lIO = lDevice.getNode('io')

        # Access the clock chip
        if lBoardType in [kBoardPC059, kBoardTLU]:
            lI2CBusNode = lIO.getNode("i2c")
            lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())
        else:
            lSIChip = lIO.getNode('pll_i2c')

        # echo("PLL Configuration id: {}".format(style(lSIChip.readConfigID(), fg='cyan')))
        # secho("PLL Information", fg='cyan')
        lConfigID = lSIChip.readConfigID()
        lVersion = collections.OrderedDict()
        lVersion['Part number'] = lSIChip.readDeviceVersion()
        lVersion['Device grade'] = lSIChip.readClockRegister(0x4)
        lVersion['Device revision'] = lSIChip.readClockRegister(0x5)
        # toolbox.printRegTable(lVersion)

        w = lSIChip.readClockRegister(0xc)

        lRegisters = collections.OrderedDict()
        lRegisters['SYSINCAL'] = decRng(w, 0)
        lRegisters['LOSXAXB'] = decRng(w, 1)
        lRegisters['XAXB_ERR'] = decRng(w, 3)
        lRegisters['SMBUS_TIMEOUT'] = decRng(w, 5)

        w = lSIChip.readClockRegister(0xd)

        lRegisters['LOS'] = decRng(w, 0, 4)
        lRegisters['OOF'] = decRng(w, 4, 4)

        w = lSIChip.readClockRegister(0xe)

        lRegisters['LOL'] = decRng(w, 1)
        lRegisters['HOLD'] = decRng(w, 5)

        w = lSIChip.readClockRegister(0xf)
        lRegisters['CAL_PLL'] = decRng(w, 5)

        w = lSIChip.readClockRegister(0x11)
        lRegisters['SYSINCAL_FLG'] = decRng(w, 0)
        lRegisters['LOSXAXB_FLG'] = decRng(w, 1)
        lRegisters['XAXB_ERR_FLG'] = decRng(w, 3)
        lRegisters['SMBUS_TIMEOUT_FLG'] = decRng(w, 5)

        w = lSIChip.readClockRegister(0x12)
        lRegisters['OOF (sticky)'] = decRng(w, 4, 4)

        # secho("PLL Status", fg='cyan')
        return (lConfigID, lVersion, lRegisters)
        # toolbox.printRegTable(lRegisters)
# ------------------------------------------------------------------------------

kFMCRev1 = 1
kFMCRev2 = 2
kPC059Rev1 = 3
kPC059FanoutHDMI = 4
kPC059FanoutSFP = 5
kTLURev1 = 6


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


    # ------------------------------------------------------------------------------
    def getAX3Slave(self):
        lIO = self.device.getNode('io')
        return lIO.getNode('uid_i2c').getSlave('AX3_Switch')
    # ------------------------------------------------------------------------------

    # ------------------------------------------------------------------------------
    def getUIDSlave(self):
        lIO = self.device.getNode('io')
        return lIO.getNode('uid_i2c').getSlave('FMC_UID_PROM')
    # ------------------------------------------------------------------------------

    # ------------------------------------------------------------------------------
    def getSIChipSlave(self):
        lIO = self.device.getNode('io')
        return lIO.getNode('pll_i2c')
    # ------------------------------------------------------------------------------

    # ------------------------------------------------------------------------------
    def resetI2CnPll(self):
        lIO = self.device.getNode('io')

        time.sleep(0.1)
            
        # PLL and I2C reset 
        lIO.getNode('csr.ctrl.pll_rst').write(0x1)
        lIO.getClient().dispatch()

        lIO.getNode('csr.ctrl.pll_rst').write(0x0)
        lIO.getClient().dispatch()
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def enableI2CSwitch(self):
        if self.info.carrierType == kCarrierEnclustraA35:
            super(FMCShell, self).enableI2CSwitch()
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
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

        echo('Resetting ' + click.style(self.device.id(), fg='blue'))

        lDevice = self.device
        lBoardType = self.info.boardType
        lCarrierType = self.info.carrierType
        lDesignType = self.info.designType

        lIO = lDevice.getNode('io')

        # Global soft reset
        self.softReset()


        time.sleep(0.1)
            
        # PLL and I2C reset 
        self.resetI2CnPll()


        # Detect the on-board eprom and read the board UID
        lUID = lIO.getNode('uid_i2c')

        echo('UID I2C Slaves')
        for lSlave in lUID.getSlaves():
            echo("  {}: {} - {}".format(lSlave, hex(lUID.getSlaveAddress(lSlave)), style('OK', fg='green') if lUID.getSlave(lSlave).ping() else style('Not responding', fg='red')))

        # Enable I2C routing
        self.enableI2CSwitch()

        lUniqueID = self.readUID()
        # echo("Timing Board PROM UID: "+style(hex(lUniqueID), fg="blue"))

        # Ensure that the board is known to the revision DB
        try:
            lRevision = self.kUIDRevisionMap[lUniqueID]
        except KeyError:
            raise click.ClickException("No revision associated to UID "+hex(lUniqueID))

        # Access the clock chip
        lSIChip = self.getSIChipSlave()
        lSIVersion = lSIChip.readDeviceVersion()
        # echo("PLL version : "+style(hex(lSIVersion), fg='blue'))

        # Ensure that the board revision has a registered clock config
        if forcepllcfg is not None:
            lFullClockConfigPath = forcepllcfg
            echo("Using PLL Clock configuration file: "+style(basename(lFullClockConfigPath), fg='green') )

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
        kPC059FanoutSFP: "wr/FANOUT_PLL_WIDEBW_SFPIN.txt",
    }

    # ------------------------------------------------------------------------------
    def getAX3Slave(self):
        lIO = self.device.getNode('io')
        return lIO.getNode('i2c').getSlave('AX3_Switch')
    # ------------------------------------------------------------------------------
    

    # ------------------------------------------------------------------------------
    def getUIDSlave(self):
        lIO = self.device.getNode('io')
        return lIO.getNode('i2c').getSlave('FMC_UID_PROM')
    # ------------------------------------------------------------------------------
    
    # ------------------------------------------------------------------------------
    def getSIChipSlave(self):
        # Access the clock chip
        lI2CBusNode = self.device.getNode("io.i2c")
        return SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())
    # ------------------------------------------------------------------------------

    
    # ------------------------------------------------------------------------------
    def resetI2CnPll(self):
        lIO = self.device.getNode('io')

        # PLL and I@C reset 
        lIO.getNode('csr.ctrl.pll_rst').write(0x1)
        lIO.getNode('csr.ctrl.rst_i2c').write(0x1)
        lIO.getNode('csr.ctrl.rst_i2cmux').write(0x1)
        lIO.getClient().dispatch()

        lIO.getNode('csr.ctrl.pll_rst').write(0x0)
        lIO.getNode('csr.ctrl.rst_i2c').write(0x0)
        lIO.getNode('csr.ctrl.rst_i2cmux').write(0x0)
        lIO.getClient().dispatch()
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def resetLockMon(self):
        lIO = self.device.getNode('io')

        lIO.getNode('csr.ctrl.rst_lock_mon').write(0x1)
        lIO.getNode('csr.ctrl.rst_lock_mon').write(0x0)
        lIO.getClient().dispatch()
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def configureSFPExpander(self):

        lI2CBusNode = self.device.getNode("io.i2c")
        lSFPExp = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('SFPExpander').getI2CAddress())

        # Set invert registers to default for both banks
        lSFPExp.setInversion(0, 0x00)
        lSFPExp.setInversion(1, 0x00)

        # BAnk 0 input, bank 1 output
        lSFPExp.setIO(0, 0x00)
        lSFPExp.setIO(1, 0xff)

        # Bank 0 - enable all SFPGs (enable low)
        lSFPExp.setOutputs(0, 0x00)
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
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


        lDevice = self.device
        lBoardType = self.info.boardType
        lCarrierType = self.info.carrierType
        lDesignType = self.info.designType

        echo('Resetting ' + click.style(lDevice.id(), fg='blue'))

        lIO = lDevice.getNode('io')

        if ( fanout ):
            secho("Fanout mode enabled", fg='green')

        # Global soft reset
        self.softReset()

        if not soft:
            
            # PLL and I@C reset 
            time.sleep(0.1)
                
            # PLL and I2C reset 
            self.resetI2CnPll()

            # Detect the on-board eprom and read the board UID
            lUID = lIO.getNode('i2c')

            echo('UID I2C Slaves')
            for lSlave in lUID.getSlaves():
                echo("  {}: {} {}".format(lSlave, hex(lUID.getSlaveAddress(lSlave)), lUID.getSlave(lSlave).ping()))

            self.enableI2CSwitch()

            lUniqueID = self.readUID()
            # echo("Timing Board PROM UID: "+style(hex(lUniqueID), fg="blue"))

            # Access the clock chip
            lSIChip = self.getSIChipSlave()
            # lSIVersion = lSIChip.readDeviceVersion()
            # echo("PLL version : "+style(hex(lSIVersion), fg='blue'))

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
                    lClockConfigPath = self.kClockConfigMap[kPC059FanoutSFP]
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

            self.configureSFPExpander()

            self.resetLockMon()

            if lDesignType == kDesingFanout:
                lDevice.getNode('switch.csr.ctrl.master_src').write(fanout)
                lIO.getNode('csr.ctrl.mux').write(0)
                lDevice.dispatch()

        echo()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
    def selectMux(self, mux):
        lDevice = self.device
        
        lDevice.getNode('io.csr.ctrl.mux').write(mux)
        lDevice.dispatch()
        secho('SFP input mux set to {}'.format(mux), fg='yellow')
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

    # ------------------------------------------------------------------------------
    def getAX3Slave(selfself):
        lIO = self.device.getNode('io')
        return lIO.getNode('i2c').getSlave('AX3_Switch')
    # ------------------------------------------------------------------------------
    #

    # ------------------------------------------------------------------------------
    def getUIDSlave(selfself):
        lIO = self.device.getNode('io')
        return lIO.getNode('i2c').getSlave('UID_PROM')
    # ------------------------------------------------------------------------------

    # ------------------------------------------------------------------------------
    def resetI2CnPll(self):
        lIO = self.device.getNode('io')
        
        # PLL and I@C reset 
        lIO.getNode('csr.ctrl.pll_rst').write(0x1)
        lIO.getNode('csr.ctrl.rst_i2c').write(0x1)
        lDevice.dispatch()

        lIO.getNode('csr.ctrl.pll_rst').write(0x0)
        lIO.getNode('csr.ctrl.rst_i2c').write(0x0)
        lDevice.dispatch()
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def resetLockMon(self):
        lIO = self.device.getNode('io')

        lIO.getNode('csr.ctrl.rst_lock_mon').write(0x1)
        lIO.getNode('csr.ctrl.rst_lock_mon').write(0x0)
        lIO.getClient().dispatch()
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def configureExpanders(self):

        lI2CBusNode = self.device.getNode("io.i2c")

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
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def configurePLLSwing(self):
        lI2CBusNode = self.device.getNode("io.i2c")
        lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())

        lSIChip.writeI2CArray(0x113, [0x9, 0x33])
    # ------------------------------------------------------------------------------

    # ------------------------------------------------------------------------------
    def configureDACs(self):
        lI2CBusNode = self.device.getNode("io.i2c")

        lDAC1 = DACSlave(lI2CBusNode, lI2CBusNode.getSlave('DAC1').getI2CAddress())
        lDAC2 = DACSlave(lI2CBusNode, lI2CBusNode.getSlave('DAC2').getI2CAddress())

        # BI signals are NIM
        lBISignalThreshold = 0x589D

        lDAC1.setInteralRef(False);
        lDAC2.setInteralRef(False);
        lDAC1.setDAC(7, lBISignalThreshold);
        lDAC2.setDAC(7, lBISignalThreshold);

        secho("DAC1 and DAC2 set to " + hex(lBISignalThreshold), fg='cyan')
    # ------------------------------------------------------------------------------

    # ------------------------------------------------------------------------------
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

        lDevice = self.device
        lBoardType = self.info.boardType
        lCarrierType = self.info.carrierType
        lDesignType = self.info.designType

        echo('Resetting ' + click.style(lDevice.id(), fg='blue'))

        lIO = lDevice.getNode('io')

        # Global soft reset
        self.softReset()

        if not (soft or lBoardType == kBoardSim):
            
            time.sleep(0.1)
            
            # PLL and I@C reset 
            self.resetI2CnPll()

            # Detect the on-board eprom and read the board UID
            lUID = lIO.getNode('i2c')

            echo('UID I2C Slaves')
            for lSlave in lUID.getSlaves():
                echo("  {}: {}".format(lSlave, hex(lUID.getSlaveAddress(lSlave))))

            # Enable I2C routing on carrier
            self.enableI2CSwitch()

            lUniqueID = self.readUID()
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

            self.configureExpanders()

            # Tweak the PLL swing
            lI2CBusNode = lDevice.getNode("io.i2c")
            self.configurePLLSwing()

            self.configureDACs()

            self.resetLockMon()

        echo()
# ------------------------------------------------------------------------------

ShellFactory.registerBoard(kBoardTLU, TLUShell)

# ------------------------------------------------------------------------------
