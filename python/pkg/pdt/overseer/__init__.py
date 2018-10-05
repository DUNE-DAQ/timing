from __future__ import print_function

import click
import time
import collections

import pdt.cli.toolbox as toolbox
import pdt.cli.definitions as defs

from click import echo, style, secho
from pdt.cli.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from pdt.cli.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from pdt.cli.definitions import kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout

from os.path import join, expandvars, basename

class Overseer(object):
    """docstring for Overseer"""
    def __init__(self, aPdtHw):
        super(Overseer, self).__init__()
        self.lDevice = aPdtHw
        self._setup()

    def _setup(self):
        lBoardInfo = toolbox.readSubNodes(self.lDevice.getNode('io.config'), False)
        self.lDevice.dispatch()
        self.boardType = lBoardInfo['board_type'].value()
        self.carrierType = lBoardInfo['carrier_type'].value()
        self.designType = lBoardInfo['design_type'].value()

        self.printme()

    def printme(self):
        print('name', self.lDevice.id())
        print('board type', self.boardType)
        print('carrier type', self.carrierType)
        print('design type', self.designType)


class Overlay(object):
    """docstring for Factory"""

    @staticmethod
    def factory(device):
        lBoardInfo = toolbox.readSubNodes(device.getNode('io.config'), False)
        device.dispatch()
        boardType = lBoardInfo['board_type'].value()
        carrierType = lBoardInfo['carrier_type'].value()
        designType = lBoardInfo['design_type'].value()

        boardcls = Overlay.makeBoardOverlay(boardType)

        def ctor(self, device, boardType, carrierType, designType):
           self.device = device
           self.boardType = boardType
           self.carrierType = carrierType
           self.designType = designType

        lCls = type(device.id(), (boardcls, ), {'__init__':ctor})

        return lCls(device, boardType, carrierType, designType)

    @staticmethod
    def makeBoardOverlay(boardType):
        if boardType == defs.kBoardFMC:
            return FMCOverlay
        elif boardType == defs.kBoardPC059:
            return PC059Overlay
        elif boardType == defs.kBoardTLU:
            return TLUOverlay


# -----------------------------------------------------------------------------
class BoardOverlay(object):
    """docstring for BoardOverlay"""
    def __init__(self):
        super(BoardOverlay, self).__init__()

    # ------------------------------------------------------------------------------
    def freq(self):
        lDevice = self.device
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

        lDevice = self.device
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

        lDevice = self.device
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
    0xd88039d980cf: kPC059Rev1,
    0xd88039d98adf: kPC059Rev1,
    0xd88039d92491: kPC059Rev1,
    0xd88039d9248e: kPC059Rev1,
    0xd88039d98ae9: kPC059Rev1,
    0xd88039d92498: kPC059Rev1,
}


class FMCOverlay(BoardOverlay):
    """docstring for PC059Overlay"""

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
    def __init__(self):
        super(PC059Overlay, self).__init__()

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

        echo('Resetting ' + click.style(self.device.id(), fg='blue'))

        lDevice = self.device
        lBoardType = self.boardType
        lCarrierType = self.carrierType
        lDesignType = self.designType

        lIO = lDevice.getNode('io')

        # if ( lBoardType == kBoardPC059 and fanout ):
        #     secho("Fanout mode enabled", fg='green')

        # Global soft reset
        lIO.getNode('csr.ctrl.soft_rst').write(0x1)
        lDevice.dispatch()


        # if not (soft or lBoardType == kBoardSim):
        
        time.sleep(1)
            
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

# ------------------------------------------------------------------------------


class PC059Overlay(BoardOverlay):
    """docstring for PC059Overlay"""
    def __init__(self):
        super(PC059Overlay, self).__init__()
                
class TLUOverlay(BoardOverlay):
    """docstring for PC059Overlay"""
    def __init__(self):
        super(PC059Overlay, self).__init__()
                

                