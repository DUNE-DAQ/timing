from __future__ import print_function

import click
import time
import collections

import timing.cli.toolbox as toolbox
import timing.common.definitions as defs

from .factory import ShellFactory, ShellContext
from click import echo, style, secho
from timing.core import I2CSlave, SI534xSlave, I2CExpanderSlave, DACSlave

from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout

from os.path import join, expandvars, basename

# -----------------------------------------------------------------------------
class BoardShell(object):
    """docstring for BoardShell"""


    # ------------------------------------------------------------------------------
    def soft_reset(self):
        lIO = self.device.getNode('io')

        # Global soft reset
        lIO.getNode('csr.ctrl.soft_rst').write(0x1)
        lIO.getClient().dispatch()
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def enableI2CSwitch(self):
        try:
            # Wake up the switch
            self.getAX3Slave().write_i2c(0x01, 0x7f)
        except RuntimeError:
            pass

        x = self.getAX3Slave().read_i2c(0x01)
        return x
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def readUID(self):
        lValues = self.getUIDSlave().read_i2cArray(0xfa, 6)
        lUniqueID = 0x0
        for lVal in lValues:
            lUniqueID = ( lUniqueID << 8 ) | lVal
        return lUniqueID
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def scanI2C(self):
        lIO = self.device.getNode('io')
        lRes = {}
        for lI2CName in self.i2cMasters:
            lRes[lI2CName] = lIO.getNode(lI2CName).scan()
        return lRes
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def pingI2CSlaves(self):
        lIO = self.device.getNode('io')
        lRes = {}
        for lI2CName in self.i2cMasters:
            lI2CNode = lIO.getNode(lI2CName)
            for lSlaveName in lI2CNode.get_slaves():
               lRes[lI2CName+'.'+lSlaveName] = (lSlaveName, lI2CNode.get_slave_address(lSlaveName), lI2CNode.get_slave(lSlaveName).ping())

        return lRes
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def measureClockFrequency(self):
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
        if lBoardType != kBoardTLU:
            lFreqs['CDR'] = freqs[1]

        return lFreqs

    # ------------------------------------------------------------------------------
    def status(self):
        def dec_rng( word, ibit, nbits=1):
            return (word >> ibit) & ((1<<nbits)-1)

        lDevice = self.device
        lIO = lDevice.getNode('io')

        # echo()
        # echo( "--- " + style("IO status", fg='cyan') + " ---")
        return toolbox.readSubNodes(lIO.getNode('csr.stat'))
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def pllstatus(self, verbose=False):
        def dec_rng( word, ibit, nbits=1):
            return (word >> ibit) & ((1<<nbits)-1)

        lDevice = self.device
        lBoardType = self.info.boardType
        lIO = lDevice.getNode('io')

        # Access the clock chip
        if lBoardType in [kBoardPC059, kBoardTLU]:
            lI2CBusNode = lIO.getNode("i2c")
            lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.get_slave('SI5345').get_i2c_address())
        else:
            lSIChip = lIO.getNode('pll_i2c')

        # echo("PLL Configuration id: {}".format(style(lSIChip.read_config_id(), fg='cyan')))
        # secho("PLL Information", fg='cyan')
        lConfigID = lSIChip.read_config_id()
        lVersion = collections.OrderedDict()
        lVersion['Part number'] = lSIChip.read_device_version()
        lVersion['Device grade'] = lSIChip.read_clock_register(0x4)
        lVersion['Device revision'] = lSIChip.read_clock_register(0x5)
        # toolbox.printRegTable(lVersion)

        w = lSIChip.read_clock_register(0xc)

        lRegisters = collections.OrderedDict()
        lRegisters['SYSINCAL'] = dec_rng(w, 0)
        lRegisters['LOSXAXB'] = dec_rng(w, 1)
        lRegisters['XAXB_ERR'] = dec_rng(w, 3)
        lRegisters['SMBUS_TIMEOUT'] = dec_rng(w, 5)

        w = lSIChip.read_clock_register(0xd)

        lRegisters['LOS'] = dec_rng(w, 0, 4)
        lRegisters['OOF'] = dec_rng(w, 4, 4)

        w = lSIChip.read_clock_register(0xe)

        lRegisters['LOL'] = dec_rng(w, 1)
        lRegisters['HOLD'] = dec_rng(w, 5)

        w = lSIChip.read_clock_register(0xf)
        lRegisters['CAL_PLL'] = dec_rng(w, 5)

        w = lSIChip.read_clock_register(0x11)
        lRegisters['SYSINCAL_FLG'] = dec_rng(w, 0)
        lRegisters['LOSXAXB_FLG'] = dec_rng(w, 1)
        lRegisters['XAXB_ERR_FLG'] = dec_rng(w, 3)
        lRegisters['SMBUS_TIMEOUT_FLG'] = dec_rng(w, 5)

        w = lSIChip.read_clock_register(0x12)
        lRegisters['OOF (sticky)'] = dec_rng(w, 4, 4)

        # secho("PLL Status", fg='cyan')
        return (lConfigID, lVersion, lRegisters)
        # toolbox.printRegTable(lRegisters)
# ------------------------------------------------------------------------------


