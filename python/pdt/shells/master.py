from __future__ import print_function

import click
import time
import collections

import pdt.cli.toolbox as toolbox
import pdt.common.definitions as defs

from .factory import ShellFactory, ShellContext
from click import echo, style, secho
from pdt.core import SI534xSlave, I2CExpanderSlave, DACSlave

from pdt.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from pdt.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from pdt.common.definitions import kDesingOverlord, kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout
from pdt.common.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap

from os.path import join, expandvars, basename

# ------------------------------------------------------------------------------
class MasterShell(object):

    kFWMajorRequired = 5

    def __init__(self):

        lMstCtx = ShellContext()

        # Tol-level nodes
        lMaster = self.device.getNode('master_top.master')
        lExtTrigNode = self.device.getNode('master_top.trig')

        lVersion = lMaster.getNode('global.version').read()
        lGenerics = toolbox.readSubNodes(lMaster.getNode('global.config'), False)
        self.device.dispatch()

        lMajor = (lVersion >> 16) & 0xff
        lMinor = (lVersion >> 8) & 0xff
        lPatch = (lVersion >> 0) & 0xff
        
        lMstCtx.version = (lVersion.value(), lMajor, lMinor, lPatch)
        lMstCtx.generics = {k:v.value() for k,v in lGenerics.items()}

        if lMajor < self.kFWMajorRequired:
            secho('ERROR: Incompatible master firmware version. Found {}, required {}'.format(hex(lMajor), hex(kMasterFWMajorRequired)), fg='red')
            raise click.Abort()

        # store references in context
        lMstCtx.masterNode = lMaster        
        lMstCtx.globalNode = lMaster.getNode('global')
        lMstCtx.aCmdNode = lMaster.getNode('acmd')
        lMstCtx.echoNode = lMaster.getNode('echo')
        lMstCtx.extTrigNode = lExtTrigNode

        self.masterCtx = lMstCtx
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def identify(self):
        echo("design '{}' on board '{}' on carrier '{}'".format(
            style(kDesignNameMap[self.info.designType], fg='blue'),
            style(kBoardNamelMap[self.info.boardType], fg='blue'),
            style(kCarrierNamelMap[self.info.carrierType], fg='blue')
        ))
        echo("Master FW rev: {}, partitions: {}, channels: {}".format(
            style(hex(self.masterCtx.version[0]), fg='cyan'),
            style(str(self.masterCtx.generics['n_part']), fg='cyan'),
            style(str(self.masterCtx.generics['n_chan']), fg='cyan'),
        ))   
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def initPartitions(self):

        for pid in range(self.masterCtx.generics['n_part']):

            lPartNode = self.masterCtx.masterNode.getNode('partition{}'.format(pid))
        
            lPartNode.reset(); 
            lPartNode.configure(0x0, False);
            lPartNode.enable();
    # ------------------------------------------------------------------------------

    # ------------------------------------------------------------------------------
    def synctime(self):

        lMaster = self.masterCtx.masterNode

        lTStampReadNode = lMaster.getNode('tstamp.ctr.val')
        lTStampWriteNode = lMaster.getNode('tstamp.ctr.set')

        lTimeStamp = lTStampReadNode.readBlock(2)
        lTStampReadNode.getClient().dispatch()

        lTime = int(lTimeStamp[0]) + (int(lTimeStamp[1]) << 32)
        secho('Old Timestamp {}'.format(hex(lTime)), fg='cyan')

        # Take the local time and split it up
        lNow = int(time.time()*defs.kSPSClockInHz)
        lNowH = (lNow >> 32) & ((1<<32)-1)
        lNowL = (lNow >> 0) & ((1<<32)-1)

        # push it on the board
        lTStampWriteNode.writeBlock([lNowL, lNowH])
        lTStampWriteNode.getClient().dispatch()

        # Read it back
        lTimeStamp = lTStampReadNode.readBlock(2)
        lTStampReadNode.getClient().dispatch()
        x = time.time()
        lTime = (int(lTimeStamp[1]) << 32) + int(lTimeStamp[0])

        secho('New Timestamp {}'.format(hex(lTime)), fg='cyan')

        print(float(lTime)/defs.kSPSClockInHz - x)

        echo ("DeltaT {}".format(toolbox.formatTStamp(lTimeStamp)))
    # ------------------------------------------------------------------------------



    # ------------------------------------------------------------------------------
    def enableEptAndWaitForReady( self, aTimeout=0.5 ):
        
        lGlobal = self.masterCtx.globalNode

        lGlobal.getNode('csr.ctrl.ep_en').write(0x0)
        lGlobal.getClient().dispatch()
        lGlobal.getNode('csr.ctrl.ep_en').write(0x1)
        lGlobal.getClient().dispatch()

        # Wait for the endpoint to be happy
        lTOutStart = time.time()
        # secho ('Return EPT reset, waiting for lock', fg='cyan')
        while time.time() < lTOutStart + aTimeout:
            time.sleep(0.1)

            lEptRdy = lGlobal.getNode('csr.stat.ep_rdy').read()
            lEptStat = lGlobal.getNode('csr.stat.ep_stat').read()
            lEptFDel = lGlobal.getNode('csr.stat.ep_fdel').read()
            lEptEdge = lGlobal.getNode('csr.stat.ep_edge').read()
            lGlobal.getClient().dispatch()

            if int(lEptRdy) == 1:
                # secho('Endpoint locked: state={}'.format(hex(lEptStat)), fg='blue')
                break
        if int(lEptRdy) == 0:
            raise RuntimeError('Failed to bring up the RTT endpoint. Current state {}'.format(hex(lEptStat)))
        # else:
            # secho ('Endpoint locked', fg='cyan')
        return (lEptFDel.value(), lEptEdge.value())
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def send_echo_and_measure_delay( self, aTimeout=0.5 ):

        lEcho = self.masterCtx.echoNode

        lEcho.getNode('csr.ctrl.go').write(0x1)
        lEcho.getClient().dispatch()
        lTOutStart = time.time()
        while time.time() < lTOutStart + aTimeout:
            time.sleep(0.1)

            lDone = lEcho.getNode('csr.stat.rx_done').read()
            lEcho.getClient().dispatch()

            if int(lDone) == 1:
                break
        if not int(lDone):
            raise RuntimeError("Timeout while waiting for echo")

        lTimeRxL = lEcho.getNode('csr.rx_l').read()
        lTimeRxH = lEcho.getNode('csr.rx_h').read()
        lTimeTxL = lEcho.getNode('csr.tx_l').read()
        lTimeTxH = lEcho.getNode('csr.tx_h').read()
        lEcho.getClient().dispatch()

        lTimeRx = (lTimeRxH << 32) + lTimeRxL
        lTimeTx = (lTimeTxH << 32) + lTimeTxL

        return lTimeTx, lTimeRx
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def pushDelay( self, aAddr, aCDel):

        lACmd = self.masterCtx.aCmdNode

        # Keep the TX sfp on
        toolbox.resetSubNodes(lACmd.getNode('csr.ctrl'), dispatch=False)
        lACmd.getNode('csr.ctrl.tx_en').write(0x1)
        lACmd.getNode('csr.ctrl.addr').write(aAddr)
        lACmd.getNode('csr.ctrl.cdel').write(aCDel)
        lACmd.getNode('csr.ctrl.update').write(0x1)
        lACmd.getNode('csr.ctrl.go').write(0x1)
        lACmd.getNode('csr.ctrl.go').write(0x0)
        lACmd.getClient().dispatch()
        secho('Coarse delay {} applied'.format(aCDel), fg='green')
    # ------------------------------------------------------------------------------


    # ------------------------------------------------------------------------------
    def enableEndpointSFP(self, aAddr, aEnable=1):
        
        lACmd = self.masterCtx.aCmdNode

        toolbox.resetSubNodes(lACmd.getNode('csr.ctrl'))
        lACmd.getNode('csr.ctrl.addr').write(aAddr)
        lACmd.getNode('csr.ctrl.tx_en').write(aEnable)
        lACmd.getNode('csr.ctrl.go').write(0x1)
        lACmd.getNode('csr.ctrl.go').write(0x0)
        lACmd.getClient().dispatch()    
    # ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------

ShellFactory.registerBoard(kDesingOverlord, MasterShell)

# ------------------------------------------------------------------------------

