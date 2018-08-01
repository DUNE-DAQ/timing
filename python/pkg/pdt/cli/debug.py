from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import pdt

import pdt.cli.toolbox as toolbox
import pdt.cli.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars
from pdt.core import SI5344Slave, SI534xSlave, I2CExpanderSlave

from pdt.cli.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from pdt.cli.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from pdt.cli.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap


# ------------------------------------------------------------------------------
@click.group('debug', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device)
def debug(obj, device):
    '''
    Timing master commands.

    DEVICE: uhal device identifier
    '''
    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)
        
    echo('Created device ' + click.style(lDevice.id(), fg='blue'))

    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lDevice.dispatch()

    # print({ k:v.value() for k,v in lBoardInfo.iteritems()})
    # raise SystemExit(0)

    echo("Design '{}' on board '{}' on carrier '{}'".format(
        style(kDesignNameMap[lBoardInfo['design_type'].value()], fg='blue'),
        style(kBoardNamelMap[lBoardInfo['board_type'].value()], fg='blue'),
        style(kCarrierNamelMap[lBoardInfo['carrier_type'].value()], fg='blue')
    ))

    obj.mDevice = lDevice
    obj.mBoardType = lBoardInfo['board_type'].value()
    obj.mCarrierType = lBoardInfo['carrier_type'].value()
    obj.mDesignType = lBoardInfo['design_type'].value()
# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
@debug.command('sfpexpander', short_help="Debug.")
@click.pass_obj
def sfpexpander(obj):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType

    if lBoardType != kBoardPC059:
        secho('No SFP expander on {}'.format(kBoardNamelMap[lBoardInfo['board_type'].value()]))
        return
    lI2CBusNode = lDevice.getNode("io.i2c")
    lSFPExp = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('SFPExpander').getI2CAddress())
    lSFPExpStatus = lSFPExp.debug()

    lLabels = [
        'B0 values', 
        'B1 values',
        'B0 enable',
        'B1 enable',
        'B0 invert',
        'B1 invert',
        'B0 I/O   ',
        'B1 I/O   ',
        ]
    for a,v in enumerate(lSFPExpStatus):
        echo("{} ({}): {}".format(lLabels[a], hex(a), hex(v)))
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@debug.command()
@click.pass_obj
@click.option('--soft-rst', 'softrst', is_flag=True)
def si5345(obj, softrst):
    def decRng( word, ibit, nbits=1):
        return (word >> ibit) & ((1<<nbits)-1)

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType

    lI2CBusNode = lDevice.getNode('io.i2c')
    lSI5345 = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())


    if softrst:
        secho("Resetting", fg='yellow')
        lSI5345.writeClockRegister(0x1e, 0x2)

    secho("Si3545 configuration id: {}".format(lSI5345.readConfigID()), fg='green')

    w = lSI5345.readClockRegister(0xc)

    registers = collections.OrderedDict()
    registers['SYSINCAL'] = decRng(w, 0)
    registers['LOSXAXB'] = decRng(w, 1)
    registers['XAXB_ERR'] = decRng(w, 3)
    registers['SMBUS_TIMEOUT'] = decRng(w, 5)

    w = lSI5345.readClockRegister(0xd)

    registers['LOS'] = decRng(w, 0, 4)
    registers['OOS'] = decRng(w, 4, 4)

    w = lSI5345.readClockRegister(0xe)

    registers['LOL'] = decRng(w, 1)
    registers['HOLD'] = decRng(w, 5)

    w = lSI5345.readClockRegister(0xf)
    registers['CAL_PLL'] = decRng(w, 5)

    w = lSI5345.readClockRegister(0x11)
    registers['SYSINCAL_FLG'] = decRng(w, 0)
    registers['LOSXAXB_FLG'] = decRng(w, 1)
    registers['XAXB_ERR_FLG'] = decRng(w, 3)
    registers['SMBUS_TIMEOUT_FLG'] = decRng(w, 5)

    for r,v in registers.iteritems():
        echo('{}: {}'.format(r,hex(v)))

    # Measure the generated clock frequency
    lFreqNode = lDevice.getNode('io.freq')
    freqs = {}
    for i in range(1 if lBoardType == kBoardTLU else 2):
        lFreqNode.getNode('ctrl.chan_sel').write(i)
        lFreqNode.getNode('ctrl.en_crap_mode').write(0)
        lFreqNode.getClient().dispatch()
        time.sleep(2)
        fq = lFreqNode.getNode('freq.count').read()
        fv = lFreqNode.getNode('freq.valid').read()
        lFreqNode.getClient().dispatch()
        freqs[i] = int(fq) * 119.20928 / 1000000 if fv else 'NaN'

    print( "Freq PLL:", freqs[0] )


# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@debug.command('inspect')
@click.argument('nodes')
@click.pass_obj
def inspect(obj, nodes):
    lDevice = obj.mDevice

    print(nodes)
    lNodeIds = lDevice.getNodes(nodes.encode('ascii','replace'))
    print(lNodeIds)
    lNodeVals = {n:lDevice.getNode(n).read() for n in lNodeIds}

    lDevice.dispatch()

    for k in sorted(lNodeVals):
        print(k,lNodeVals[k])
# ------------------------------------------------------------------------------

