
from __future__ import print_function

import click
import time
import collections

import timing.cli.toolbox as toolbox
import timing.common.definitions as defs

from .factory import ShellFactory, ShellContext
from .master import MasterShell
from click import echo, style, secho
from timing.core import SI534xSlave, I2CExpanderSlave, DACSlave

from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.common.definitions import kDesingOverlord, kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout
from timing.common.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap

from os.path import join, expandvars, basename


# ------------------------------------------------------------------------------
class FanoutShell(MasterShell):

    def scanports(self):

        lDevice = self.device
        lGlobal = self.masterCtx.globalNode
        lBoardType = self.info.boardType

        if lBoardType != kBoardPC059:
            raise RuntimeError('Mux is only available on PC059 boards')

        lLocked = []
        for mux in range(0,8):
            secho('Scanning slot {}'.format(mux), fg='cyan')

            lDevice.getNode('io.csr.ctrl.mux').write(mux)
            lDevice.dispatch()
            # echo('SFP input mux set to {}'.format(mux))

            try:
                self.enableEptAndWaitForReady()
                lState = lGlobal.getNode('csr.stat.ep_stat').read()
                lFDel = lGlobal.getNode('csr.stat.ep_fdel').read()
                lEdge = lGlobal.getNode('csr.stat.ep_edge').read()
                lGlobal.getClient().dispatch()

                secho('Endpoint locked: state={}, fdel={}, edge={}'.format(hex(lState), hex(lFDel), hex(lEdge)), fg='blue')
                lLocked.append(mux)
            except RuntimeError as e:
                secho('Slot {}: no lock - {}'.format(mux,e), fg='yellow')

        echo()
        if lLocked:
            secho('Locked slots {}'.format(','.join( ( str(l) for l in lLocked))), fg='green')
        else:
            secho('No slots locked', fg='red')
        return lLocked

ShellFactory.registerBoard(kDesingFanout, FanoutShell)

# ------------------------------------------------------------------------------
