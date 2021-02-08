from __future__ import print_function

import click
import time
import collections

from .boards import BoardShell
from .factory import ShellFactory

from os.path import join, expandvars, basename
from click import echo, style, secho
from pdt.common.definitions import kBoardPC059
from pdt.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from pdt.common.definitions import kDesingFanout
from pdt.core import I2CSlave, SI534xSlave, I2CExpanderSlave, DACSlave


# ------------------------------------------------------------------------------
class PC059Shell(BoardShell):

    kPC059Rev1 = 3
    kPC059FanoutSFP = 5

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

    i2cMasters=[
        'i2c',
        'usfp_i2c',
    ]
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

            # Enable the i2c switch on the board
            self.enableI2CSwitch()

            lUniqueID = self.readUID()
            # echo("Timing Board PROM UID: "+style(hex(lUniqueID), fg="blue"))

            # Access the clock chip
            lSIChip = self.getSIChipSlave()

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
                    lClockConfigPath = self.kClockConfigMap[self.kPC059FanoutSFP]
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
        # secho('SFP input mux set to {}'.format(mux), fg='yellow')
# ------------------------------------------------------------------------------

ShellFactory.registerBoard(kBoardPC059, PC059Shell)

# ------------------------------------------------------------------------------
                