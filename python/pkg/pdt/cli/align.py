from __future__ import print_function
import click
import time
import StringIO
import traceback

# PDT imports
import pdt.cli.toolbox as toolbox
import pdt.common.definitions as defs

from click import echo, style, secho
from pdt.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from pdt.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from pdt.common.definitions import kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout
from pdt.common.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap

from master import master

# ------------------------------------------------------------------------------
@master.group('align', short_help='Endpoint alignment command group.')
@click.pass_obj
def align(obj):
    lMaster = obj.mMaster
    obj.mGlobal = lMaster.getNode('global')
    obj.mACmd = lMaster.getNode('acmd')
    obj.mEcho = lMaster.getNode('echo')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def enableEptAndWaitForReady( aGlobal, aTimeout=0.5 ):
    aGlobal.getNode('csr.ctrl.ep_en').write(0x0)
    aGlobal.getClient().dispatch()
    aGlobal.getNode('csr.ctrl.ep_en').write(0x1)
    aGlobal.getClient().dispatch()

    # Wait for the endpoint to be happy
    lTOutStart = time.time()
    secho ('Return EPT reset, waiting for lock', fg='cyan')
    while time.time() < lTOutStart + aTimeout:
        time.sleep(0.1)

        lEptStat = aGlobal.getNode('csr.stat.ep_stat').read()
        lEptRdy = aGlobal.getNode('csr.stat.ep_rdy').read()
        aGlobal.getClient().dispatch()

        # print ('stat', hex(lEptStat))
        # print ('rdy ', hex(lEptRdy))
        if int(lEptRdy) == 1:
            secho('Endpoint locked: state={}'.format(hex(lEptStat)), fg='blue')
            break
    if int(lEptRdy) == 0:
        raise RuntimeError('Failed to bring up the RTT endpoint. Current state {}'.format(hex(lEptStat)))
    else:
        secho ('Endpoint locked', fg='cyan')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def sendEchoAndMeasureDelay( aEcho, aTimeout=0.5 ):
    aEcho.getNode('csr.ctrl.go').write(0x1)
    aEcho.getClient().dispatch()
    lTOutStart = time.time()
    while time.time() < lTOutStart + aTimeout:
        time.sleep(0.1)

        lDone = aEcho.getNode('csr.stat.rx_done').read()
        aEcho.getClient().dispatch()

        print ('done', hex(lDone))
        if int(lDone) == 1:
            break
    if not int(lDone):
        raise RuntimeError("Timeout while waiting for echo")

    lTimeRxL = aEcho.getNode('csr.rx_l').read()
    lTimeRxH = aEcho.getNode('csr.rx_h').read()
    lTimeTxL = aEcho.getNode('csr.tx_l').read()
    lTimeTxH = aEcho.getNode('csr.tx_h').read()
    aEcho.getClient().dispatch()

    lTimeRx = (lTimeRxH << 32) + lTimeRxL
    lTimeTx = (lTimeTxH << 32) + lTimeTxL

    return lTimeTx, lTimeRx
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def pushDelay( aACmd, aAddr, aCDel, aFDel):
    # Keep the TX sfp on
    toolbox.resetSubNodes(aACmd.getNode('csr.ctrl'), dispatch=False)
    aACmd.getNode('csr.ctrl.tx_en').write(0x1)
    aACmd.getNode('csr.ctrl.addr').write(aAddr)
    aACmd.getNode('csr.ctrl.cdel').write(aCDel)
    aACmd.getNode('csr.ctrl.fdel').write(aFDel)
    aACmd.getNode('csr.ctrl.update').write(0x1)
    aACmd.getNode('csr.ctrl.go').write(0x1)
    aACmd.getNode('csr.ctrl.go').write(0x0)
    aACmd.getClient().dispatch()
    secho('Coarse delay {} applied'.format(aCDel), fg='green')
    secho('Fine delay {} applied'.format(aFDel), fg='green')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def enableEndpointSFP(aACmd, aAddr, aEnable=1):
    toolbox.resetSubNodes(aACmd.getNode('csr.ctrl'))
    aACmd.getNode('csr.ctrl.addr').write(aAddr)
    aACmd.getNode('csr.ctrl.tx_en').write(aEnable)
    aACmd.getNode('csr.ctrl.go').write(0x1)
    aACmd.getNode('csr.ctrl.go').write(0x0)
    aACmd.getClient().dispatch()    
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('apply-delay', short_help="Control the trigger return endpoint")
@click.argument('addr', type=toolbox.IntRange(0x0,0x100))
@click.argument('delay', type=toolbox.IntRange(0x0,0x32))
@click.argument('fdelay', type=toolbox.IntRange(0x0,0xf))
@click.option('--mux', '-m', type=click.IntRange(0,7), help='Mux select')
@click.option('--force', '-f', is_flag=True, default=False, help='Skip RTT measurement')
@click.pass_obj
@click.pass_context
def applydelay(ctx, obj, addr, delay, fdelay, mux, force):

    lDevice = obj.mDevice
    lACmd = obj.mACmd
    lEcho = obj.mEcho
    lGlobal = obj.mGlobal
    lBoardType = obj.mBoardType

    if mux is not None:
        if lBoardType == kBoardPC059:
            lDevice.getNode('io.csr.ctrl.mux').write(mux)
            echo('SFP input mux set to {}'.format(mux))
        else:
            raise RuntimeError('Mux is only available on PC059 boards')

    try:
        if not force:
            # Switch off all TX SFPs
            # enableEndpointSFP(lACmd, 0x0, False)
            # Turn on the current target
            # enableEndpointSFP(lACmd, addr)

            time.sleep(0.1)

            enableEptAndWaitForReady(lGlobal)

            lCsrDump = toolbox.readSubNodes(lGlobal.getNode('csr'))
            print(toolbox.formatRegTable(lCsrDump, False))

            lTimeTx, lTimeRx = sendEchoAndMeasureDelay(lEcho)

            lCsrDump = toolbox.readSubNodes(lGlobal.getNode('csr'))
            print(toolbox.formatRegTable(lCsrDump, False))

            print(lTimeTx, lTimeRx, lTimeRx-lTimeTx)
            #------------

            lGlobal.getNode('csr.ctrl.ep_en').write(0x0)

        pushDelay(lACmd, addr, delay, fdelay)

        if not force:
            time.sleep(1)

            enableEptAndWaitForReady(lGlobal)

            #------------
            lTimeTx, lTimeRx = sendEchoAndMeasureDelay(lEcho)

            #------------
            print(lTimeTx, lTimeRx, lTimeRx-lTimeTx)
    except Exception:
        lExc = StringIO.StringIO()
        traceback.print_exc(file=lExc)
        print ("Exception in user code:")
        print ('-'*60)
        secho(lExc.getvalue(), fg='red')
        print ('-'*60)


    enableEndpointSFP(lACmd, 0x0, False)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('measure-delay', short_help="Control the trigger return endpoint")
@click.argument('addr', type=toolbox.IntRange(0x0,0x100))
@click.option('--mux', '-m', type=click.IntRange(0,7), help='Mux select (fanout only)')
@click.pass_obj
@click.pass_context
def measuredelay(ctx, obj, addr, mux):

    lDevice = obj.mDevice
    lACmd = obj.mACmd
    lEcho = obj.mEcho
    lGlobal = obj.mGlobal
    lBoardType = obj.mBoardType


    if mux is not None:
        if lBoardType == kBoardPC059:
            lDevice.getNode('io.csr.ctrl.mux').write(mux)
            lDevice.dispatch()
            echo('SFP input mux set to {}'.format(mux))
        else:
            raise RuntimeError('Mux is only available on PC059 boards')

    # Switch off all TX SFPs
    # enableEndpointSFP(lACmd, 0x0, False)
    time.sleep(0.1)
    # Turn on the current target
    # enableEndpointSFP(lACmd, addr)

    time.sleep(0.1)

    enableEptAndWaitForReady(lGlobal)

    lTimeTx, lTimeRx = sendEchoAndMeasureDelay(lEcho)
    print(lTimeTx, lTimeRx, lTimeRx-lTimeTx)

    # enableEndpointSFP(lACmd, addr, 0x0)

# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('toggle-tx', short_help="Control the trigger return endpoint")
@click.argument('addr', type=toolbox.IntRange(0x0,0x100))
@click.option('--on/--off', default=True, help='enable/disable tx')
@click.pass_obj
def toggletx(obj, addr, on):

    lACmd = obj.mACmd

    enableEndpointSFP(lACmd, addr, on)
    # toolbox.resetSubNodes(lACmd.getNode('csr.ctrl'))

    # lACmd.getNode('csr.ctrl.addr').write(addr)
    # lACmd.getNode('csr.ctrl.tx_en').write(on)
    # lACmd.getNode('csr.ctrl.go').write(0x1)
    # lACmd.getNode('csr.ctrl.go').write(0x0)
    # lACmd.getClient().dispatch()

    lDone = lACmd.getNode('csr.stat.done').read()
    lACmd.getClient().dispatch()

    print (hex(lDone))
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('scan-mux', short_help="Control the trigger return endpoint")
@click.pass_obj
def scanmux(obj):

    lDevice = obj.mDevice
    lGlobal = obj.mGlobal
    lBoardType = obj.mBoardType

    if lBoardType != kBoardPC059:
        raise RuntimeError('Mux is only available on PC059 boards')

    lLocked = []
    for mux in xrange(0,8):
        secho('Scanning slot {}'.format(mux), fg='cyan')

        lDevice.getNode('io.csr.ctrl.mux').write(mux)
        lDevice.dispatch()
        # echo('SFP input mux set to {}'.format(mux))

        try:
            enableEptAndWaitForReady(lGlobal)
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

# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('switch-n-lock', short_help="Control the trigger return endpoint")
@click.option('--mux', '-m', type=click.IntRange(0,7), help='Mux select (fanout only)')
@click.pass_obj
def switchnlock(obj, mux):
    
    lDevice = obj.mDevice
    lGlobal = obj.mGlobal
    lBoardType = obj.mBoardType

    if mux is not None:
        if lBoardType == kBoardPC059:
            lDevice.getNode('io.csr.ctrl.mux').write(mux)
            lMux = lDevice.getNode('io.csr.ctrl.mux').read()
            lDevice.dispatch()
            echo('SFP input mux set to {}'.format(lMux))
        else:
            raise RuntimeError('Mux is only available on PC059 boards')

    enableEptAndWaitForReady(lGlobal)

    lCsrDump = toolbox.readSubNodes(lGlobal.getNode('csr'))
    print(toolbox.formatRegTable(lCsrDump, False))
