from __future__ import print_function

import click
import time
import collections

import pdt.cli.toolbox as toolbox
import pdt.common.definitions as defs

from factory import ShellFactory, ShellContext
from click import echo, style, secho
from pdt.core import I2CSlave, SI534xSlave, I2CExpanderSlave, DACSlave

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
            for lSlaveName in lI2CNode.getSlaves():
               lRes[lI2CName+'.'+lSlaveName] = (lSlaveName, lI2CNode.getSlaveAddress(lSlaveName), lI2CNode.getSlave(lSlaveName).ping())

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


