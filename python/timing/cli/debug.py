from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import timing

import timing.cli.toolbox as toolbox
import timing.common.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars
from timing.core import SI534xSlave, I2CExpanderSlave, LTC2945Node


from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU, kBoardMIB, kBoardGIB, kBoardPC069, kBoardFIB
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.common.definitions import kBoardNameMap, kCarrierNameMap, kDesignNameMap


# ------------------------------------------------------------------------------
@click.group('debug', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, shell_complete=toolbox.completeDevices)
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

    # print({ k:v.value() for k,v in lBoardInfo.items()})
    # raise SystemExit(0)

    echo("Design '{}' on board '{}' on carrier '{}'".format(
        style(kDesignNameMap[lBoardInfo['design_type'].value()], fg='blue'),
        style(kBoardNameMap[lBoardInfo['board_type'].value()], fg='blue'),
        style(kCarrierNameMap[lBoardInfo['carrier_type'].value()], fg='blue')
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
    lIO = lDevice.getNode('io')

    # Detect the on-board eprom and read the board UID
    if lBoardType in [kBoardPC059, kBoardTLU, kBoardMIB, kBoardGIB]:
        lUID = lDevice.getNode('io.i2c')
    else:
        lUID = lDevice.getNode('io.uid_i2c')

    lPROMSlave = 'UID_PROM' if lBoardType in [kBoardTLU,kBoardMIB,kBoardGIB] else 'FMC_UID_PROM'

    if lBoardType == kBoardGIB:
        lDevice.getNode("io.csr.ctrl.i2c_sw_rst").write(0x0)
        lDevice.dispatch()
        lDevice.getNode("io.csr.ctrl.i2c_sw_rst").write(0x1)
        lDevice.dispatch()
        print("switch reset")
    #    lIO.set_i2c_mux_channels(0x1)

    lValues = lUID.get_slave(lPROMSlave).read_i2cArray(0xfa, 6)
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
        secho('No SFP expander on {}'.format(kBoardNameMap[lBoardInfo['board_type'].value()]))
        return
    lI2CBusNode = lDevice.getNode("io.i2c")
    lSFPExp = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.get_slave('SFPExpander').get_i2c_address())
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
def scan_i2c(obj):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')
    
    lNodes = []
    lSwitches={}
    lSwitchChannels={}
    if lBoardType in [kBoardFMC, kBoardPC069]:
        lNodes = ['io.sfp_i2c','io.uid_i2c','io.pll_i2c']
    elif lBoardType == kBoardPC059:
        lNodes = ['io.i2c', 'io.usfp_i2c']
        switch_address=lDevice.getNode('io.i2c').get_slave_address('SFP_Switch')
        lSwitches={"io.i2c": switch_address}
        lSwitchChannels={"io.i2c": 8}
    elif lBoardType in [kBoardTLU, kBoardMIB, kBoardGIB]:
        lNodes = ['io.i2c']
        if lBoardType == kBoardGIB:
            lSwitches={"io.i2c": 0x70}
            lSwitchChannels={"io.i2c": 7}
    else:
        secho(f"Error I don't know about board {lBoardType} : {kBoardNameMap[lBoardType]}", fg='red')
    # if lBoardType == kBoardPC059:
    #     lSFPSwitch = lDevice.getNode('io.i2c').get_slave('SFP_Switch')
    #     print(lSFPSwitch.read_i2cPrimitive(1))
    #     print(lSFPSwitch.write_i2cPrimitive([0x1]))
    #     print(lSFPSwitch.read_i2cPrimitive(1))

    for n in lNodes:
        lI2CBusNode = lDevice.getNode(n)
        echo('Scanning '+style(n,fg='cyan'))
        print("reset switch")
        lDevice.getNode("io.csr.ctrl.i2c_sw_rst").write(0x0)
        lDevice.dispatch()
        lDevice.getNode("io.csr.ctrl.i2c_sw_rst").write(0x1)
        lDevice.dispatch()
        lAddresses = lI2CBusNode.scan()
        print("  '{}': {} devices found.\n  Addresses: {}".format(n, len(lAddresses), ', '.join((hex(a) for a in lAddresses))))

        if n in lSwitches:
            for switch,address in lSwitches.items():
                print (f"switch {switch} address: {address}")
                switch_channels=lSwitchChannels[switch]
                print(f"working with {switch}, @ adr {address}, it has {switch_channels} channels")

                for channel in range(0,switch_channels):
                    secho(f"Scanning with channel {channel} enabled", fg='cyan')
                    try:
                        lI2CBusNode.write_i2cPrimitive(address, [1<<channel])
                    except:
                        secho(f"failure configuring switch {address}")
                    lAddresses = lI2CBusNode.scan()
                    print("  '{}': {} devices found.\n  Addresses: {}".format(n, len(lAddresses), ', '.join((hex(a) for a in lAddresses))))

# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@debug.command('pll', short_help="Debug.")
@click.pass_obj
def pll(obj):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')

    lI2CBusNode = lDevice.getNode('io.i2c')
    lDevice.getNode("io.csr.ctrl.i2c_sw_rst").write(0x1)
    lDevice.getNode("io.csr.ctrl.clk_gen_rst").write(0x1)
    lDevice.dispatch()

    lI2CBusNode.write_i2cPrimitive(0x70, [1])

    lSIChip = SI534xSlave(lI2CBusNode, 0x68)
    #lSIVersion = lSIChip.read_device_version()

    lSIVersion = lSIChip.read_device_version()
    echo(f"PLL version {hex(lSIVersion)}")
# ------------------------------------------------------------------------------

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
    lSwitchSlave = lI2CBusNode.get_slave('SFP_Switch')
    print(lSwitchSlave.ping())

    # print(lSwitchSlave.read_i2cPrimitive(1))
    # time.sleep(0.1)

    lSwitchSlave.write_i2cPrimitive([3])
    print(lSwitchSlave.read_i2cPrimitive(1))
    print(lSwitchSlave.read_i2cPrimitive(1))
    print(lSwitchSlave.read_i2cPrimitive(1))

# ------------------------------------------------------------------------------
@debug.command('sfp-status', short_help="Debug.")
@click.pass_obj
def sfp_status(obj):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType

    if lBoardType == kBoardFMC:
        lSFPNodeName = 'io.sfp_i2c'
        lSFPLabel = 'SFP'
    elif lBoardType == kBoardPC059:
        lSFPNodeName = 'io.usfp_i2c'
        lSFPLabel = 'USFP'
    else:
        secho('No SFP on {}'.format(kBoardNameMap[lBoardType]))
        return

    lI2CBusNode = lDevice.getNode(lSFPNodeName)
    lEEPromSlave = lI2CBusNode.get_slave('SFP_EEProm')
    lDiagSlave = lI2CBusNode.get_slave('SFP_Diag')

    readSFPStatus(lEEPromSlave, lDiagSlave, lSFPLabel)

    if lBoardType == kBoardPC059:
        lDevice.getNode('io.csr.ctrl.rst_i2cmux').write(0x1)
        lDevice.dispatch()
        lDevice.getNode('io.csr.ctrl.rst_i2cmux').write(0x0)
        lDevice.dispatch()
        time.sleep(0.1)

        lI2CBusNode = lDevice.getNode('io.i2c')
        lSwitchSlave = lI2CBusNode.get_slave('SFP_Switch')
        lEEPromSlave = lI2CBusNode.get_slave('SFP_EEProm')
        lDiagSlave = lI2CBusNode.get_slave('SFP_Diag')

        # lOld = lSwitchSlave.read_i2cPrimitive(1)
        # lSwitchSlave.write_i2cPrimitive([0x0])
        time.sleep(0.1)
        for iSFP in range(8):
            try:
                lSwitchSlave.write_i2cPrimitive([1 << iSFP])
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
def readSFPStatus(aEEProm, aDiag, aLabel):

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
    lVenInfoEnc['Name'] = asciidecode(aEEProm.read_i2cArray(20,16))
    lVenInfoEnc['OUI'] = '{}.{}.{}'.format(*(aEEProm.read_i2cArray(37,3)))
    lVenInfoEnc['Part Number'] = asciidecode(aEEProm.read_i2cArray(40,16))
    lVenInfoEnc['Revision'] = asciidecode(aEEProm.read_i2cArray(56,4))
    lVenInfoEnc['Serial Number'] = asciidecode(aEEProm.read_i2cArray(68,16))
    lVenInfoEnc['Day'] = asciidecode(aEEProm.read_i2cArray(88,2))
    lVenInfoEnc['Month'] = asciidecode(aEEProm.read_i2cArray(86,2))
    lVenInfoEnc['Year'] = asciidecode(aEEProm.read_i2cArray(84,2))

    secho("{} Vendor info".format(aLabel), fg='cyan')
    # for k,v in lVenInfoEnc.items():
    #     v = ''.join([chr(c) for c in v])
    #     echo(' - '+k+': '+style(v, fg='cyan'))
    echo(toolbox.formatDictTable(lVenInfoEnc, aHdr=False, aSort=False))
    echo()
    lLaserWl = aEEProm.read_i2cArray(60,2)
    lLaserWl = (lLaserWl[0] << 8) + lLaserWl[1]
    echo('Laser Wavelength: '+style(str(lLaserWl)+'nm', fg='cyan'))

    lRegs = collections.OrderedDict()

    lRegs['Identifier'] = aEEProm.read_i2c(0)
    lRegs['Ext Identifier'] = aEEProm.read_i2c(1)
    lRegs['Connector'] = aEEProm.read_i2c(2)

    # Transciever Compatinility
    lTransComp = aEEProm.read_i2cArray(3, 8)

    lRegs['Encoding'] = aEEProm.read_i2c(11)
    lRegs['BR, Nominal'] = aEEProm.read_i2c(12)
    lRegs['Rate ID'] = aEEProm.read_i2c(13)

    toolbox.printRegTable(lRegs, aHeader=False, sort=False)

    echo()
    secho("{} Diagnostic info".format(aLabel), fg='cyan')

    lReadings = collections.OrderedDict()

    lReadings['Temp'] = tempdecode(aDiag.read_i2cArray(96, 2))
    lReadings['Vcc']  = vccdecode(aDiag.read_i2cArray(98, 2))
    lReadings['TX bias']  = biascurdecode(aDiag.read_i2cArray(100, 2))
    lReadings['TX power']  = powerdecode(aDiag.read_i2cArray(102, 2))
    lReadings['RX power']  = powerdecode(aDiag.read_i2cArray(104, 2))
    lMiscStatus = aDiag.read_i2c(110)
    lReadings['TX disable'] = (lMiscStatus >> 7) & 0x1

    # for k,v in lReadings.items():
        # print (k, v)

    echo(toolbox.formatDictTable(lReadings, aHdr=False, aSort=False))
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@debug.command('lm75-temp-read', short_help="Read temp data from a LM75.")
@click.pass_obj
def lm75_temp_read(obj):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')

    if lBoardType != kBoardFIB:
        secho(f'Only FIB (v2) supported. Board type is {lBoardType}', fg='red')
        return

    i2c_bus = lDevice.getNode('io.i2c')

    lValues = i2c_bus.get_slave('TEMP_MON').read_i2cPrimitive(2)
    temp_raw = (lValues[1] & 0x80) >> 7
    temp_raw = temp_raw | (lValues[0] << 1)
    echo(f"LM75 temp data")
    echo(f" raw bytes: {hex(lValues[0])}, {hex(lValues[1])}")
    echo(f" combined word: {hex(temp_raw)}")
    echo(f" temp [C]: {toolbox.twos_complement(temp_raw,9)*0.5}")
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@debug.command('ltc2945', short_help="Read data from a LTC2945.")
@click.pass_obj
def lm75_temp_read(obj):

    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lIO = lDevice.getNode('io')

    if lBoardType != kBoardGIB:
        secho(f'Only GIB (v2 (not checked)) supported. Board type is {lBoardType}', fg='red')
        return

    i2c_bus = lDevice.getNode('io.i2c')

    voltage_power_mon_i2c_addr_map = {"5": 0x67, "3.3": 0x6a, "2.5": 0x69}
    voltage_sense_resistance_map   = {"5": 0.005, "3.3": 0.02, "2.5": 0.02}

    for v,addr in voltage_power_mon_i2c_addr_map.items():
        sense_r = voltage_sense_resistance_map[v]
        lLTC2945 = LTC2945Node(i2c_bus, addr, sense_r)

        lLTC2945.write_i2c(0x0, 0x05)

        delta_sense_v = lLTC2945.read_delta_sense_v()
        v_in = lLTC2945.read_v_in()
        power = lLTC2945.read_power()

        echo(f"LTC2945 {v}V data is:")
        echo(f"deltaSenseV : {delta_sense_v*1000} mV")
        echo(f"Vin : {v_in} V")
        echo(f"power is: {power*1000} mW\n")

    #lLTC2945_5v = LTC2945Node(i2c_bus, ic_addr_5v_mon, 0.005)
    #lLTC2945_3v3 = LTC2945Node(i2c_bus, ic_addr_3v3_mon, 0.02)

    #lLTC2945_5v.write_i2c(0x0, 0x05)
    #lLTC2945_3v3.write_i2c(0x0, 0x05)

    #delta_sense_v_5v = lLTC2945_5v.read_delta_sense_v()
    #echo(f"LTC2945 5V deltaSense V is: {delta_sense_v_5v*1000} mV\n")

    #delta_sense_v_3v3 = lLTC2945_3v3.read_delta_sense_v()
    #echo(f"LTC2945 3.3V deltaSense V is: {delta_sense_v_3v3*1000} mV\n")

    #v_in_5v = lLTC2945_5v.read_v_in()
    #echo(f"LTC2945 5V Vin is: {v_in_5v} V\n")

    #v_in_3v3 = lLTC2945_3v3.read_v_in()
    #echo(f"LTC2945 3.3V Vin is: {v_in_3v3} V\n")

    #power_5v = lLTC2945_5v.read_power()
    #echo(f"LTC2945 5V power is: {power_5v*1000} mW\n")

    #power_3v3 = lLTC2945_3v3.read_power()
    #echo(f"LTC2945 3.3V power is: {power_3v3*1000} mW\n")

    #regs=i2c_bus.read_i2cArray(ic_addr_5v_mon, 0x0, 40, True)
    #echo(f"LTC2945 5V regs: {regs}\n")

    #v_in_raw = (regs[0x1e] & 0xf) << 4
    #v_in_raw = v_in_raw | (regs[0x1f] >> 4)

    #v_in_resolution = 0.025
    #echo("LTC2945 Vin data - ")
    #echo(f"raw bytes: {regs[0x1e]} {regs[0x1f]}")
    #echo(f"combined word: {v_in_raw}")
    #echo(f"Vin [V]: {v_in_raw*v_in_resolution}")
# ------------------------------------------------------------------------------