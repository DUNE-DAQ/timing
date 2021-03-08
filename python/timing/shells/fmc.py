from __future__ import print_function

import click
import time
import collections

from .boards import BoardShell
from .factory import ShellFactory

from os.path import join, expandvars, basename
from click import echo, style, secho
from timing.common.definitions import kBoardSim, kBoardFMC
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.core import I2CSlave, SI534xSlave


# ------------------------------------------------------------------------------
class FMCShell(BoardShell):
    """docstring for PC059Shell"""
    kFMCRev1 = 1
    kFMCRev2 = 2

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

    i2cMasters=[
        'uid_i2c',
        'sfp_i2c',
        'pll_i2c'
    ]

    # ------------------------------------------------------------------------------
    def getAX3Slave(self):
        lIO = self.device.getNode('io')
        return lIO.getNode('uid_i2c').get_slave('AX3_Switch')
    # ------------------------------------------------------------------------------

    # ------------------------------------------------------------------------------
    def getUIDSlave(self):
        lIO = self.device.getNode('io')
        return lIO.getNode('uid_i2c').get_slave('FMC_UID_PROM')
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
        self.soft_reset()

        if not soft:
            time.sleep(0.1)
                
            # PLL and I2C reset 
            self.resetI2CnPll()

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
            echo("SI354x configuration id: {}".format(style(lSIChip.read_config_id(), fg='green')))

# ------------------------------------------------------------------------------

ShellFactory.registerBoard(kBoardSim, FMCShell)
ShellFactory.registerBoard(kBoardFMC, FMCShell)