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
from timing.core import SI534xSlave, I2CExpanderSlave, LTC2945Node, I2C9546SwitchSlave


from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU, kBoardMIB, kBoardGIB, kBoardGIBV3, kBoardPC069, kBoardFIB
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.common.definitions import ClockSource
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
    if lBoardType in [kBoardPC059, kBoardTLU, kBoardMIB, kBoardGIB, kBoardGIBV3]:
        lUID = lDevice.getNode('io.i2c')
    else:
        lUID = lDevice.getNode('io.uid_i2c')

    lPROMSlave = 'UID_PROM' if lBoardType in [kBoardTLU,kBoardMIB,kBoardGIB,kBoardGIBV3] else 'FMC_UID_PROM'

    if lBoardType in [kBoardGIB, kBoardGIBV3]:
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
    elif lBoardType in [kBoardTLU, kBoardMIB, kBoardGIB,kBoardGIBV3]:
        lNodes = ['io.i2c']
        if lBoardType == kBoardGIBV3:
            lSwitches={"io.i2c": 0x70}
            lSwitchChannels={"io.i2c": 8}
        elif lBoardType == kBoardGIB:
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

    if lBoardType not in [kBoardGIB, kBoardGIBV3]:
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

# ------------------------------------------------------------------------------
@debug.group('gib', short_help='GIB debugging command group.')
@click.option('--force', '-f', is_flag=True, default=False,
              help="Run when GIB isn't detected")
@click.pass_obj
def gib(obj, force):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType

    if lBoardType not in [kBoardGIB, kBoardGIBV3]:
        if force:
            secho('Warning, only designed for GIB(vX) testing. '
                  + 'Continuing due to --force flag...')
        else:
            secho('Only designed for GIB(vX) testing. Stopping...')
            return
    obj.mI2CBus = lDevice.getNode('io.i2c')
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@gib.command('reset-switch', short_help="Debug.")
@click.pass_obj
def reset_switch(obj):
    lDevice = obj.mDevice
    lI2CSwitch = I2C9546SwitchSlave(obj.mI2CBus, obj.mI2CBus.get_slave('I2CSwitch').get_i2c_address())

    echo(f"Initial switch status:\t{lI2CSwitch.read_channels_states():08b}")
    lI2CSwitch.enable_channel(0)
    echo(f"Enabled channel 0:\t{lI2CSwitch.read_channels_states():08b}")
    echo()

    echo("reset switch")
    lDevice.getNode("io.csr.ctrl.i2c_sw_rst").write(0x0)
    lDevice.dispatch()
    lDevice.getNode("io.csr.ctrl.i2c_sw_rst").write(0x1)
    lDevice.dispatch()

    echo(f"State after reset:\t{lI2CSwitch.read_channels_states():08b}")
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@gib.command('reset-expander', short_help="Debug.")
@click.pass_obj
def reset_expander(obj):
    lDevice = obj.mDevice
    expander0 = I2CExpanderSlave(obj.mI2CBus, obj.mI2CBus.get_slave('SFPExpander0').get_i2c_address())
    expander1 = I2CExpanderSlave(obj.mI2CBus, obj.mI2CBus.get_slave('SFPExpander1').get_i2c_address())

    conf_00 = expander0.read_outputs_config(0)
    conf_01 = expander0.read_outputs_config(1)
    conf_10 = expander1.read_outputs_config(0)
    conf_11 = expander1.read_outputs_config(1)

    print("Expected state : 0x{00}{ff}{ff}{ff}")
    print("{00}: Expander 1 bus 1 is output")
    print("{ff}: Expander 1 bus 0 is intput")
    print("{ff}: Expander 0 bus 1 is intput")
    print("{ff}: Expander 0 bus 0 is intput\n")

    print(f"Initial output state: {conf_11:#02x}{conf_10:02x}{conf_01:02x}{conf_00:02x}")

    print("reset expander")
    lDevice.getNode("io.csr.ctrl.i2c_exten_rst").write(0x0)
    lDevice.dispatch()
    lDevice.getNode("io.csr.ctrl.i2c_exten_rst").write(0x1)
    lDevice.dispatch()

    conf_00_rst = expander0.read_outputs_config(0)
    conf_01_rst = expander0.read_outputs_config(1)
    conf_10_rst = expander1.read_outputs_config(0)
    conf_11_rst = expander1.read_outputs_config(1)

    print(f"Output state after reset: {conf_11_rst:#02x}{conf_10_rst:02x}{conf_01_rst:02x}{conf_00_rst:02x}")

    print("reconfigure expander")
    lDevice.getNode('io').configure_expander()

    conf_00_rcf = expander0.read_outputs_config(0)
    conf_01_rcf = expander0.read_outputs_config(1)
    conf_10_rcf = expander1.read_outputs_config(0)
    conf_11_rcf = expander1.read_outputs_config(1)

    print(f"Output state after reconfiguring: {conf_11_rcf:#02x}{conf_10_rcf:02x}{conf_01_rcf:02x}{conf_00_rcf:02x}")
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@gib.command('monitor-exp-intr', short_help="Debug.")
@click.option('--no-end', is_flag=True, default=False,
              help="Continue monitoring indefintely (requires manual interrupt).")
@click.option('--no-exp-read', is_flag=True, default=False,
              help="Prevents reading out the expander status.")
@click.pass_obj
def monitor_exp_intr(obj, no_end, no_exp_read):
    lDevice = obj.mDevice
    lIO = lDevice.getNode("io")

    def format_exp(exp_data):
        bus_10 = f"{(exp_data>>16)%(2**8):08b}"
        bus_01 = f"{(exp_data>>8)%(2**8):08b}"
        bus_00 = f"{exp_data%(2**8):08b}"
        return f"[10]{bus_10} [01]{bus_01} [00]{bus_00}"

    num_seconds = 10

    intr0 = lDevice.getNode("io.csr.stat.i2c_exten_intr0")
    intr1 = lDevice.getNode("io.csr.stat.i2c_exten_intr1")

    init_i0 = intr0.read()
    init_i1 = intr1.read()
    lDevice.dispatch()
    print(f"Initial status:\nInterrupt 0: {init_i0}\nInterrupt 1: {init_i1}")
    if not no_exp_read:
        init_exp = lIO.read_io_expanders()
        print(f"Expander ([bus]contents): {format_exp(init_exp)}\n")

    changed = False

    if no_end:
        while_cond = lambda t: True
        print("Monitoring interrupt line indefinitely "
              + "(requires manual interruption)...")
    else:
        print(f"Monitoring interrupt line for {num_seconds}s...")
        t_end = time.time() + num_seconds
        while_cond = lambda t: t < t_end
    while while_cond(time.time()):
        i0 = intr0.read()
        i1 = intr1.read()
        lDevice.dispatch()
        if not no_exp_read:
            exp = lIO.read_io_expanders()
            time.sleep(1e-1) # Reduce probability that change occurs
                    # between reading interrupt line and expander state
            exp_change = exp != init_exp
        else:
            exp_change = False
        
        i0_change = i0 != init_i0
        i1_change = i1 != init_i1
        intr_change = i0_change or i1_change

        if exp_change or intr_change:
            if not no_end:
                changed = True
                break
            else:
                if i0_change:
                    print(f"INT0 changed: was {init_i0}, now {i0}")
                    init_i0 = i0
                if i1_change:
                    print(f"INT1 changed: was {init_i1}, now {i1}")
                    init_i1 = i1
                if exp_change and (not no_exp_read):
                    print(f"Expander changed: was {format_exp(init_exp)},")
                    print(f"                  now {format_exp(exp)}")
                    init_exp = exp

    if changed:
        if i0_change:
            print(f"INT0 changed: now {i0}")
        if i1_change:
            print(f"INT1 changed: now {i1}")
        if exp_change:
            print(f"Expander changed: now {format_exp(exp)}")
            if not intr_change:
                print("No change in interrupt seen after change in expander")
                print("This could be due to the change occuring between "
                      + "reading the interrupt lines and reading the "
                      + "expander state - multiple tests recommended.")
        time.sleep(0.1)
        i0_post = intr0.read()
        i1_post = intr1.read()
        lDevice.dispatch()
        print("\nAfter reading expander:")
        if i0 != i0_post:
            print(f"INT0 changed: now {i0_post}")
        if i1 != i1_post:
            print(f"INT1 changed: now {i1_post}")
        if (i0 == i0_post) and (i1 == i1_post):
            print("No change in interrupt lines")
            print(" ^ UNEXPECTED BEHAVIOUR ^")
    else:
        print("No changes observed.")
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@gib.command('reset-pll', short_help="Debug.")
@click.option('--clock-source', 'clocksource',
              type=click.Choice(ClockSource.__members__.keys()),
              help='Manually specify clock source, free-running, upstream, etc..')
@click.pass_obj
def reset_pll(obj, clocksource):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType
    lDesignType = obj.mDesignType
    lIO = lDevice.getNode("io")
    lPLL = lIO.get_pll()

    if clocksource is None:
        lClockSource = toolbox.get_default_clock_source(
            lDesignType, lBoardType)
        print(f"Default clock config selected for {kDesignNameMap[lDesignType]} on {kBoardNameMap[lBoardType]} is: {lClockSource}", fg='yellow')
    else:
        lClockSource=ClockSource.__members__[clocksource]

    print(("#"*30) + "\n      Initial PLL status\n" + ("#"*30))
    lPLL.get_status(True)

    init_conf_id = lPLL.read_config_id()
    print(f"\nFound configuration {init_conf_id} prior to reset.\n")
    print("Setting reset line LOW...\n")
    lIO.getNode("csr.ctrl.clk_gen_rst").write(0x0)
    lDevice.dispatch()

    print(("#"*30) + "\n      Reset line set LOW\n" + ("#"*30))
    try:
        lPLL.get_status(True)
    except Exception as e:
        print("SUCCESS - PLL can't be ready with reset low, exception:\n" + str(e))

    print("\nSetting reset line HIGH...\n")
    lIO.getNode("csr.ctrl.clk_gen_rst").write(0x1)
    lDevice.dispatch()
    conf_id = lPLL.read_config_id()
    print(f"\nFound configuration {init_conf_id} after reset.\n")
    time.sleep(1)
    lIO.configure_pll(lIO.get_full_clock_config_file_path(lClockSource))
    assert lPLL.read_config_id() == init_conf_id, "New PLL configuration does not match initial configuration, io reset advised"

    print(("#"*30) + "\n     Post-reset PLL status\n" + ("#"*30))
    lPLL.get_status(True)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@gib.command('monitor-pll-intr', short_help="Debug.")
@click.option('--no-end', is_flag=True, default=False,
              help="Continue monitoring indefintely (requires manual interrupt).")
@click.pass_obj
def monitor_pll_intr(obj, no_end):
    lDevice = obj.mDevice
    lIO = lDevice.getNode("io")
    lPLL = lIO.get_pll()

    num_seconds = 10

    pll_intr = lDevice.getNode("io.csr.stat.clk_gen_intr")

    init_ipll = pll_intr.read()
    lDevice.dispatch()
    print(("#"*30) + "\n      Initial PLL status\n" + ("#"*30))
    lPLL.get_status(True)
    print(f"\nPLL Interrupt: {init_ipll}\n")

    changed = False

    if no_end:
        while_cond = lambda t: True
        print("Monitoring interrupt line indefinitely "
              + "(requires manual interruption)...")
    else:
        print(f"Monitoring interrupt line for {num_seconds}s...")
        t_end = time.time() + num_seconds
        while_cond = lambda t: t < t_end
    while while_cond(time.time()):
        ipll = pll_intr.read()
        lDevice.dispatch()

        pll_change = ipll != init_ipll

        if pll_change:
            if not no_end:
                changed = True
                break
            else:
                print(("-"*30) + "\nInterrupt change found. New PLL status:")
                lPLL.get_status(True)
                print(f"\nInterrupt was {init_ipll}, now {ipll}\n")
                init_ipll = ipll

    if changed:
        print("\nInterrupt found\n")
        print(("#"*30) + "\n        New PLL status\n" + ("#"*30))
        lPLL.get_status(True)
        print(f"\nPLL Interrupt: was {init_ipll}, now {ipll}\n")
        
        ipll_post = pll_intr.read()
        lDevice.dispatch()
        print("\nAfter reading the PLL:")
        if ipll == ipll_post:
            print("No change in interrupt observed")
        else:
            print(f"\nPLL Interrupt changed: now {ipll_post}\n")
    else:
        print("No changes observed.")
# ------------------------------------------------------------------------------
