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
from pdt.core import SI534xSlave, I2CExpanderSlave


from pdt.cli.master import freq
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
@debug.command('inspect')
@click.argument('nodes')
@click.pass_obj
def inspect(obj, nodes):
    lDevice = obj.mDevice

    lNodeIds = lDevice.getNodes(nodes.encode('ascii','replace'))
    lNodeVals = {n:lDevice.getNode(n).read() for n in lNodeIds}

    lDevice.dispatch()

    toolbox.printRegTable(lNodeVals, False)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@debug.command()
@click.pass_context
def ipy(ctx):
    '''
    Start an interactive IPython session.

    The board HwInterface is accessible as 'lDevice'
    '''
    lDevice = ctx.obj.mDevice

    from IPython import embed
    embed()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@debug.command('uid', short_help="Unique ID reader.")
@click.pass_obj
def uuid(obj):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType

    # Detect the on-board eprom and read the board UID
    if lBoardType in [kBoardPC059, kBoardTLU]:
        lUID = lDevice.getNode('io.i2c')
    else:
        lUID = lDevice.getNode('io.uid_i2c')

    lPROMSlave = 'UID_PROM' if lBoardType == kBoardTLU else 'FMC_UID_PROM'
    lValues = lUID.getSlave(lPROMSlave).readI2CArray(0xfa, 6)
    lUniqueID = 0x0
    for lVal in lValues:
        lUniqueID = ( lUniqueID << 8 ) | lVal
    echo("Timing Board PROM UID: "+style(hex(lUniqueID), fg="blue"))
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
@debug.command('scan-i2c', short_help="Debug.")
@click.pass_obj
def sfp_status(obj):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType

    lNodes = []

    if lBoardType == kBoardFMC:
        lNodes = ['io.sfp_i2c','io.uid_i2c','io.pll_i2c']
    elif lBoardType == kBoardPC059:
        lNodes = ['io.i2c', 'io.usfp_i2c']
    elif lBoardType == kBoardTLU:
        lNodes = ['io.i2c']
    
    # if lBoardType == kBoardPC059:
    #     lSFPSwitch = lDevice.getNode('io.i2c').getSlave('SFP_Switch')
    #     print(lSFPSwitch.readI2CPrimitive(1))
    #     print(lSFPSwitch.writeI2CPrimitive([0x1]))
    #     print(lSFPSwitch.readI2CPrimitive(1))

    for n in lNodes:
        lI2CBusNode = lDevice.getNode(n)
        echo('Scanning '+style(n,fg='cyan'))
        lAddresses = lI2CBusNode.scan()
        print("  '{}': {} devices found.\n  Addresses: {}".format(n, len(lAddresses), ', '.join((hex(a) for a in lAddresses))))

# ------------------------------------------------------------------------------

@debug.command('fanout-sfp-scan', short_help="Debug.")
@click.pass_obj
def fanout_sfpscan(obj):
    
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType

    if lBoardType != kBoardPC059:
        print('Wrong board', lBoardType, 'sorry mate')
        return


    lDevice.getNode('io.csr.ctrl.rst_i2cmux').write(0x1)
    lDevice.dispatch()
    lDevice.getNode('io.csr.ctrl.rst_i2cmux').write(0x0)
    lDevice.dispatch()
    time.sleep(1)

    lSFPNodeName = 'io.i2c'
    lI2CBusNode = lDevice.getNode(lSFPNodeName)
    lSwitchSlave = lI2CBusNode.getSlave('SFP_Switch')
    print(lSwitchSlave.ping())

    # print(lSwitchSlave.readI2CPrimitive(1))
    # time.sleep(0.1)

    lSwitchSlave.writeI2CPrimitive([3])
    print(lSwitchSlave.readI2CPrimitive(1))
    print(lSwitchSlave.readI2CPrimitive(1))
    print(lSwitchSlave.readI2CPrimitive(1))

# ------------------------------------------------------------------------------
@debug.command('sfp-status', short_help="Debug.")
@click.pass_obj
def sfp_status(obj):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType

    if lBoardType == kBoardFMC:
        lSFPNodeName = 'io.sfp_i2c'
    elif lBoardType == kBoardPC059:
        lSFPNodeName = 'io.usfp_i2c'
    else:
        secho('No SFP on {}'.format(kBoardNamelMap[lBoardType]))
        return

    lI2CBusNode = lDevice.getNode(lSFPNodeName)
    lEEPromSlave = lI2CBusNode.getSlave('eeprom')
    lDiagSlave = lI2CBusNode.getSlave('diag')

    readSFPStatus(lEEPromSlave, lDiagSlave)

    if lBoardType == kBoardPC059:
        lDevice.getNode('io.csr.ctrl.rst_i2cmux').write(0x1)
        lDevice.dispatch()
        lDevice.getNode('io.csr.ctrl.rst_i2cmux').write(0x0)
        lDevice.dispatch()
        time.sleep(0.1)

        lI2CBusNode = lDevice.getNode('io.i2c')
        lSwitchSlave = lI2CBusNode.getSlave('SFP_Switch')
        lEEPromSlave = lI2CBusNode.getSlave('SFP_EEProm')
        lDiagSlave = lI2CBusNode.getSlave('SFP_Diag')

        # lOld = lSwitchSlave.readI2CPrimitive(1)
        # lSwitchSlave.writeI2CPrimitive([0x0])
        time.sleep(0.1)
        for iSFP in xrange(8):
            try:
                lSwitchSlave.writeI2CPrimitive([1 << iSFP])
            except RuntimeError as lExc:
                pass
            if not lEEPromSlave.ping():
                secho('SFP {} not available'.format(iSFP),fg='yellow')
                continue
            else:
                secho('SFP {} found'.format(iSFP),fg='green')
                # readSFPStatus(lEEPromSlave, lDiagSlave)

# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
def readSFPStatus(aEEProm, aDiag):

    def asciidecode( v ): 
        return (''.join([chr(c) for c in v])).rstrip()

    def tempdecode( v ):
        lSign = -1 if ((v[0]>>7) & 0x1) else 1
        x = lSign*((v[0] & 0x7f) + v[1]/float(0xff))
        return '{:.3f} C'.format(x)

    def vccdecode( v ):
        return '{:.4f} V'.format(((v[0] << 8) + v[1])/float(10000))

    def biascurdecode( v ):
        return '{:.3f} mA'.format(((v[0] << 8) + v[1])*2e-3)

    def powerdecode( v ):
        return '{:.3f} mW'.format(((v[0] << 8) + v[1])*1e-3)


    echo()
    lVenInfoEnc = collections.OrderedDict()
    lVenInfoEnc['Name'] = asciidecode(aEEProm.readI2CArray(20,16))
    lVenInfoEnc['OUI'] = '{}.{}.{}'.format(*(aEEProm.readI2CArray(37,3)))
    lVenInfoEnc['Part Number'] = asciidecode(aEEProm.readI2CArray(40,16))
    lVenInfoEnc['Revision'] = asciidecode(aEEProm.readI2CArray(56,4))
    lVenInfoEnc['Serial Number'] = asciidecode(aEEProm.readI2CArray(68,16))
    lVenInfoEnc['Day'] = asciidecode(aEEProm.readI2CArray(88,2))
    lVenInfoEnc['Month'] = asciidecode(aEEProm.readI2CArray(86,2))
    lVenInfoEnc['Year'] = asciidecode(aEEProm.readI2CArray(84,2))

    secho("SFP Vendor info", fg='cyan')
    # for k,v in lVenInfoEnc.items():
    #     v = ''.join([chr(c) for c in v])
    #     echo(' - '+k+': '+style(v, fg='cyan'))
    echo(toolbox.formatDictTable(lVenInfoEnc, aHdr=False, aSort=False))
    echo()
    lLaserWl = aEEProm.readI2CArray(60,2)
    lLaserWl = (lLaserWl[0] << 8) + lLaserWl[1]
    echo('Laser Wavelength: '+style(str(lLaserWl)+'nm', fg='cyan'))

    lRegs = collections.OrderedDict()

    lRegs['Identifier'] = aEEProm.readI2C(0)
    lRegs['Ext Identifier'] = aEEProm.readI2C(1)
    lRegs['Connector'] = aEEProm.readI2C(2)

    # Transciever Compatinility
    lTransComp = aEEProm.readI2CArray(3, 8)

    lRegs['Encoding'] = aEEProm.readI2C(11)
    lRegs['BR, Nominal'] = aEEProm.readI2C(12)
    lRegs['Rate ID'] = aEEProm.readI2C(13)

    toolbox.printRegTable(lRegs, aHeader=False, sort=False)

    echo()
    secho("SFP Diagnostic info", fg='cyan')

    lReadings = collections.OrderedDict()

    lReadings['Temp'] = tempdecode(aDiag.readI2CArray(96, 2))
    lReadings['Vcc']  = vccdecode(aDiag.readI2CArray(98, 2))
    lReadings['TX bias']  = biascurdecode(aDiag.readI2CArray(100, 2))
    lReadings['TX power']  = powerdecode(aDiag.readI2CArray(102, 2))
    lReadings['Rx power']  = powerdecode(aDiag.readI2CArray(104, 2))
    lMiscStatus = aDiag.readI2C(110)
    lReadings['TX disable'] = (lMiscStatus >> 7) & 0x1

    # for k,v in lReadings.iteritems():
        # print (k, v)

    echo(toolbox.formatDictTable(lReadings, aHdr=False, aSort=False))
# ------------------------------------------------------------------------------
