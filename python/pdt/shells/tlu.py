from __future__ import print_function

import click
import time
import collections

from .boards import BoardShell
from .factory import ShellFactory

from os.path import join, expandvars, basename
from click import echo, style, secho
from pdt.common.definitions import kBoardTLU, kBoardSim
# from pdt.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from pdt.common.definitions import kDesingFanout
from pdt.core import I2CSlave, SI534xSlave, I2CExpanderSlave, DACSlave



# ------------------------------------------------------------------------------
class TLUShell(BoardShell):
    """docstring for PC059Shell"""
    kTLURev1 = 6

    kUIDRevisionMap = {
        0x5410ecbb9426: kTLURev1,
    }

    kClockConfigMap = {
        kTLURev1: "wr/TLU_EXTCLK_10MHZ_NOZDM.txt"
    }
    
    i2cMasters=[
        'i2c',
    ]
    # ------------------------------------------------------------------------------
    def getAX3Slave(self):
        lIO = self.device.getNode('io')
        return lIO.getNode('i2c').getSlave('AX3_Switch')
    # ------------------------------------------------------------------------------
    #

    # ------------------------------------------------------------------------------
    def getUIDSlave(self):
        lIO = self.device.getNode('io')
        return lIO.getNode('i2c').getSlave('UID_PROM')
    # ------------------------------------------------------------------------------

    # ------------------------------------------------------------------------------
    def resetI2CnPll(self):
        lIO = self.device.getNode('io')
        
        # PLL and I@C reset 
        lIO.getNode('csr.ctrl.pll_rst').write(0x1)
        lIO.getNode('csr.ctrl.rst_i2c').write(0x1)
        lIO.getClient().dispatch()

        lIO.getNode('csr.ctrl.pll_rst').write(0x0)
        lIO.getNode('csr.ctrl.rst_i2c').write(0x0)
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

            # Enable I2C routing on carrier
            self.enableI2CSwitch()

            lUniqueID = self.readUID()

            try:
                lRevision = self.kUIDRevisionMap[lUniqueID]
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
                    lClockConfigPath = self.kClockConfigMap[lRevision]    
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
# ------------------------------------------------------------------------------

ShellFactory.registerBoard(kBoardTLU, TLUShell)

# ------------------------------------------------------------------------------
