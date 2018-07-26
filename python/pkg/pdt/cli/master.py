from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import math
import pdt

import pdt.cli.toolbox as toolbox
import pdt.cli.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars
from pdt.core import SI5344Slave, SI534xSlave, I2CExpanderSlave

kMasterFWMajorRequired = 4

kBoardSim = 0x1
kBoardFMC = 0x0
kBoardPC059 = 0x2
kBoardMicrozed = 0x3
kBoardTLU = 0x4
# kBoardKC705 = 'kc705'

kCarrierEnclustraA35 = 0x0
kCarrierKC705 = 0x1
kCarrierMicrozed = 0x2


kBoardNamelMap = {
    kBoardSim: 'sim',
    kBoardFMC: 'fmc',
    kBoardPC059: 'pc059',
    kBoardMicrozed: 'microzed',
    kBoardTLU: 'tlu'
}

kCarrierNamelMap = {
    kCarrierEnclustraA35: 'enclustra-a35',
    kCarrierKC705: 'kc705',
    kCarrierMicrozed: 'microzed',
}

# ------------------------------------------------------------------------------
#    __  ___         __         
#   /  |/  /__ ____ / /____ ____
#  / /|_/ / _ `(_-</ __/ -_) __/
# /_/  /_/\_,_/___/\__/\__/_/   
                        

@click.group('mst', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device)
def master(obj, device):
    '''
    Timing master commands.

    DEVICE: uhal device identifier
    '''
    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)
        
    echo('Created device ' + click.style(lDevice.id(), fg='blue'))

    lMaster = lDevice.getNode('master_top.master')

    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lVersion = lMaster.getNode('global.version').read()
    lGenerics = toolbox.readSubNodes(lMaster.getNode('global.config'), False)
    lDevice.dispatch()

    # print({ k:v.value() for k,v in lBoardInfo.iteritems()})
    # raise SystemExit(0)

    lMajor = (lVersion >> 16) & 0xff
    lMinor = (lVersion >> 8) & 0xff
    lPatch = (lVersion >> 0) & 0xff
    echo("Master FW version: {}, board  '{}' on '{}'".format(
        hex(lVersion), 
        style(kBoardNamelMap[lBoardInfo['board_type'].value()], fg='blue'),
        style(kCarrierNamelMap[lBoardInfo['carrier_type'].value()], fg='blue')
    ))
    echo("Design {} - chans: {}, parts: {}".format(hex(lBoardInfo['design_type']), lGenerics['n_chan'], lGenerics['n_part']))

    if lMajor < 4:
        secho('ERROR: Incompatible master firmware version. Found {}, required {}'.format(hex(lMajor), hex(kMasterFWMajorRequired)), fg='red')
        raise click.Abort()

    obj.mDevice = lDevice
    obj.mMaster = lMaster
    
    obj.mGenerics = { k:v.value() for k,v in lGenerics.iteritems()}
    obj.mVersion = lVersion.value()
    obj.mBoardType = lBoardInfo['board_type'].value()
    obj.mCarrierType = lBoardInfo['carrier_type'].value()
    obj.mDesignType = lBoardInfo['design_type'].value()
    
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@master.command()
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

kFMCRev1 = 1
kFMCRev2 = 2
kPC059Rev1 = 3
kPC059Fanout = 4
kTLURev1 = 5

kClockConfigMap = {
    kFMCRev1: "SI5344/PDTS0000.txt",
    kFMCRev2: "SI5344/PDTS0003.txt",
    kPC059Rev1: "SI5345/PDTS0005.txt",
    kPC059Fanout: "devel/PDTS_PC059_FANOUT.txt",
    kTLURev1: "devel/PDTS_TLU_MASTER.txt"
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

# kUIDRevisionMap = {
# }


# ------------------------------------------------------------------------------
@master.command('reset', short_help="Perform a hard reset on the timing master.")
@click.option('--soft', '-s', is_flag=True, default=False, help='Soft reset i.e. skip the clock chip configuration.')
@click.option('--fanout', is_flag=True, default=False, help='Configures the board in fanout mode (pc059 only)')
@click.pass_obj
def reset(obj, soft, fanout):
    '''
    Perform a hard reset on the timing master, including

    \b
    - ipbus registers
    - i2c buses
    - pll and pll configuration
    '''

    echo('Resetting ' + click.style(obj.mDevice.id(), fg='blue'))

    lDevice = obj.mDevice
    lMaster = obj.mMaster
    lBoardType = obj.mBoardType
    lCarrierType = obj.mCarrierType

    # Global soft reset
    lDevice.getNode('io.csr.ctrl.soft_rst').write(0x1)
    lDevice.dispatch()


    if not (soft or lBoardType == kBoardSim):
        
        time.sleep(1)
        
        # PLL and I@C reset 
        lDevice.getNode('io.csr.ctrl.pll_rst').write(0x1)
        if lBoardType in [kBoardPC059, kBoardTLU]:
            lDevice.getNode('io.csr.ctrl.rst_i2c').write(0x1)
            lDevice.getNode('io.csr.ctrl.rst_i2cmux').write(0x1)

        if lBoardType == kBoardPC059:
            lDevice.getNode('io.csr.ctrl.master_src').write(fanout)

        lDevice.dispatch()

        lDevice.getNode('io.csr.ctrl.pll_rst').write(0x0)
        if lBoardType in [kBoardPC059, kBoardTLU]:
            lDevice.getNode('io.csr.ctrl.rst_i2c').write(0x0)
            lDevice.getNode('io.csr.ctrl.rst_i2cmux').write(0x0)
        lDevice.dispatch()


        # Detect the on-board eprom and read the board UID
        if lBoardType in [kBoardPC059, kBoardTLU]:
            lUID = lDevice.getNode('io.i2c')
        else:
            lUID = lDevice.getNode('io.uid_i2c')

        echo('UID I2C Slaves')
        for lSlave in lUID.getSlaves():
            echo("  {}: {}".format(lSlave, hex(lUID.getSlaveAddress(lSlave))))

        if (
            lBoardType == kBoardTLU or
            lBoardType == kBoardPC059 or
            (lBoardType == kBoardFMC and lCarrierType == kCarrierEnclustraA35)
            ):
            lUID.getSlave('AX3_Switch').writeI2C(0x01, 0x7f)
            x = lUID.getSlave('AX3_Switch').readI2C(0x01)
            echo("I2C enable lines: {}".format(x))
        elif lCarrierType == kCarrierKC705:
            lUID.getSlave('KC705_Switch').writeI2CPrimitive([0x10])
            # x = lUID.getSlave('KC705_Switch').readI2CPrimitive(1)
            echo("KC705 I2C switch enabled (hopefully)".format())
        else:
            click.ClickException("Unknown board kind {}".format(lBoardType))



        # If not a TLU, read the unique ID from the prom 
        if lBoardType != kBoardTLU:
            lValues = lUID.getSlave('FMC_UID_PROM').readI2CArray(0xfa, 6)
            lUniqueID = 0x0
            for lVal in lValues:
                lUniqueID = ( lUniqueID << 8 ) | lVal
            echo("Timing Board PROM UID: "+style(hex(lUniqueID), fg="blue"))


            # Ensure that the board is known to the revision DB
            try:
                lRevision = kUIDRevisionMap[lUniqueID]
            except KeyError:
                raise click.ClickException("No revision associated to UID "+hex(lUniqueID))

        # Access the clock chip
        if lBoardType in [kBoardPC059, kBoardTLU]:
            lI2CBusNode = lDevice.getNode("io.i2c")
            lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('SI5345').getI2CAddress())
        else:
            lSIChip = lDevice.getNode('io.pll_i2c')
        lSIVersion = lSIChip.readDeviceVersion()
        echo("PLL version : "+style(hex(lSIVersion), fg='blue'))

        # Ensure that the board revision has a registered clock config
        if lBoardType == kBoardTLU:
            lClockConfigPath = kClockConfigMap[kTLURev1]
        elif lBoardType == kBoardPC059 and fanout:
            secho("Overriding clock config - fanout mode", fg='green')
            lClockConfigPath = kClockConfigMap[kPC059Fanout]
        else:
            try:
                lClockConfigPath = kClockConfigMap[lRevision]    
            except KeyError:
                raise ClickException("Board revision " << lRevision << " has no associated clock configuration")

        echo("Clock configuration to load "+style(lClockConfigPath, fg='green') )

        # Configure the clock chip
        lFullClockConfigPath = expandvars(join('${PDT_TESTS}/etc/clock', lClockConfigPath))
        lSIChip.configure(lFullClockConfigPath)

        if lBoardType == kBoardPC059:
            lDevice.getNode('io.csr.ctrl.cdr_edge').write(1)
            lDevice.getNode('io.csr.ctrl.sfp_edge').write(1)
            lDevice.getNode('io.csr.ctrl.hdmi_edge').write(0)
            lDevice.getNode('io.csr.ctrl.usfp_edge').write(1)
            lDevice.getNode('io.csr.ctrl.mux').write(0)
            lDevice.dispatch()

        # Measure the generated clock frequency
        for i in range(1 if lBoardType == kBoardTLU else 2):
            lDevice.getNode("io.freq.ctrl.chan_sel").write(i)
            lDevice.getNode("io.freq.ctrl.en_crap_mode").write(0)
            lDevice.dispatch()
            time.sleep(2)
            fq = lDevice.getNode("io.freq.freq.count").read()
            fv = lDevice.getNode("io.freq.freq.valid").read()
            lDevice.dispatch()
            print( "Freq:", i, int(fv), int(fq) * 119.20928 / 1000000 )
        
        if lBoardType == kBoardFMC:
            lDevice.getNode("io.csr.ctrl.sfp_tx_dis").write(0)
            lDevice.dispatch()
        elif lBoardType == kBoardPC059:
            lI2CBusNode = lDevice.getNode("io.i2c")
            lSFPExp = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('SFPExpander').getI2CAddress())

            # Set invert registers to default for both banks
            lSFPExp.setInversion(0, 0x00)
            lSFPExp.setInversion(1, 0x00)

            # BAnk 0 input, bank 1 output
            lSFPExp.setIO(0, 0x00)
            lSFPExp.setIO(1, 0xff)

            # Bank 0 - enable all SFPGs (enable low)
            lSFPExp.setOutputs(0, 0x00)
            secho("SFPs 0-7 enabled", fg='cyan')
        elif lBoardType == kBoardTLU:
            # # #I2C EXPANDER CONFIGURATION BEGIN
            # IC6=PCA9539PW(master_I2C, 0x74)
            # #BANK 0
            # IC6.setInvertReg(0, 0x00)# 0= normal
            # IC6.setIOReg(0, 0x00)# 0= output 
            # IC6.setOutputs(0, 0x00) <<<<<<<<<<<<<<<<<<<
            # res= IC6.getInputs(0)
            # print "\tIC6 read back bank 0: 0x%X" % res[0]
            # #
            # #BANK 1
            # IC6.setInvertReg(1, 0x00)# 0= normal
            # IC6.setIOReg(1, 0x00)# 0= output 
            # IC6.setOutputs(1, 0x80) <<<<<<<<<<<<<<<<<<<
            # res= IC6.getInputs(1)
            # print "\tIC6 read back bank 1: 0x%X" % res[0]

            # # # #
            # IC7=PCA9539PW(master_I2C, 0x75)
            # #BANK 0
            # IC7.setInvertReg(0, 0x00)# 0= normal
            # IC7.setIOReg(0, 0x00)# 0= output 
            # IC7.setOutputs(0, 0xF0) <<<<<<<<<<<<<<<<<<<
            # res= IC7.getInputs(0)
            # print "\tIC7 read back bank 0: 0x%X" % res[0]
            # #
            # #BANK 1
            # IC7.setInvertReg(1, 0x00)# 0= normal
            # IC7.setIOReg(1, 0x00)# 0= output 
            # IC7.setOutputs(1, 0xF0) <<<<<<<<<<<<<<<<<<<
            # res= IC7.getInputs(1)
            # print "\tIC7 read back bank 1: 0x%X" % res[0]
            # # #I2C EXPANDER CONFIGURATION END

            lExp1 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('Expander1').getI2CAddress())
            lExp2 = I2CExpanderSlave(lI2CBusNode, lI2CBusNode.getSlave('Expander2').getI2CAddress())

            # Bank 0
            lExp1.setInversion(0, 0x00)
            lExp1.setIO(0, 0x00)
            lExp1.setOutputs(0, 0x00)

            # Bank 1
            lExp1.setInversion(1, 0x00)
            lExp1.setIO(1, 0x00)
            lExp1.setOutputs(1, 0x80)


            # Bank 0
            lExp2.setInversion(0, 0x00)
            lExp2.setIO(0, 0x00)
            lExp2.setOutputs(0, 0xF0)

            # Bank 1
            lExp2.setInversion(1, 0x00)
            lExp2.setIO(1, 0x00)
            lExp2.setOutputs(1, 0xF0)
        else:
            click.ClickException("Unknown board kind {}".format(lBoardType))



    # Reset controls
    lDevice.getNode("io.csr.ctrl.rst").write(1)
    lDevice.dispatch()
    lDevice.getNode("io.csr.ctrl.rst").write(0)
    lDevice.dispatch()

    lScmdGenNode = lMaster.getNode('scmd_gen')

    echo()
    echo("--- Global status ---")
    lCsrStat = toolbox.readSubNodes(lMaster.getNode('global.csr.stat'))
    for k,v in lCsrStat.iteritems():
        echo("{}: {}".format(k, hex(v)))
    echo()
# ------------------------------------------------------------------------------




# ------------------------------------------------------------------------------
@master.command('freq', short_help="Measure some frequencies.")
@click.pass_obj
def freq(obj):
    lDevice = obj.mDevice
    lBoardType = obj.mBoardType

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

# ------------------------------------------------------------------------------
@master.group('part', invoke_without_command=True)
@click.pass_obj
@click.argument('id', type=int, callback=toolbox.validate_partition)
def partition(obj, id):
    """
    Partition specific commands

    ID: Id of the selected partition
    """
    obj.mPartitionId = id
    try:
        obj.mPartitionNode = obj.mMaster.getNode('partition{}'.format(id))
    except Exception as lExc:
        click.Abort('Partition {} not found in address table'.format(id))

# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@partition.command('monitor', short_help='Display the status of the timing master.')
@click.pass_obj
@click.option('--watch', '-w', is_flag=True, default=False, help='Turn on automatic refresh')
@click.option('--period','-p', type=click.IntRange(0, 240), default=2, help='Period of automatic refresh')
def monitor(obj, watch, period):
    '''
    Display the master status, accepted and rejected command counters
    '''

    # lDevice = obj.mDevice
    lMaster = obj.mMaster
    lPartId = obj.mPartitionId
    lPartNode = obj.mPartitionNode
    lNumChan = obj.mGenerics['n_chan']

    lTStampNode = lMaster.getNode('tstamp.ctr')

    while(True):
        if watch:
            click.clear()
        
        echo()
        echo( "-- " + style("Master state", fg='green') + "---")
        echo()

        lScmdGenNode = lMaster.getNode('scmd_gen')
        lScmdGenNode.getNode('sel').write(lPartId)
        lScmdGenNode.getClient().dispatch()

        secho( "=> Time sync generator", fg='green')
        lScmdGenCtrlDump = toolbox.readSubNodes(lScmdGenNode.getNode('ctrl'))
        for n in sorted(lScmdGenCtrlDump):
            echo( "  {} {}".format(n, hex(lScmdGenCtrlDump[n])))

        echo()
        secho( "=> Cmd generator control", fg='green')

        lScmdGenChanCtrlDump = toolbox.readSubNodes(lScmdGenNode.getNode('chan_ctrl'))
        for n in sorted(lScmdGenChanCtrlDump):
            echo( "  {} {}".format(n, hex(lScmdGenChanCtrlDump[n])))
        echo()

        echo()
        toolbox.printCounters( lScmdGenNode, {
            'actrs': 'Accept counters',
            'rctrs': 'Reject counters',
            }, lNumChan)

        # secho( "=> Spill generator control", fg='green')
        # lDump = toolbox.readSubNodes(lDevice.getNode('master.spill.csr.ctrl'))
        # for n in sorted(lDump):
        #     echo( "  {} {}".format(n, hex(lDump[n])))
        # echo()
        # secho( "=> Spill generator stats", fg='green')
        # lDump = toolbox.readSubNodes(lDevice.getNode('master.spill.csr.stat'))
        # for n in sorted(lDump):
        #     echo( "  {} {}".format(n, hex(lDump[n])))
        # echo()

        secho( "=> Partition {}".format(lPartId), fg='green')

        lCtrlDump = toolbox.readSubNodes(lPartNode.getNode('csr.ctrl'))
        lStatDump = toolbox.readSubNodes(lPartNode.getNode('csr.stat'))

        echo( "Control registers" )
        for n in sorted(lCtrlDump):
            echo( "  {} {}".format(n, hex(lCtrlDump[n])))
        echo()
        echo( "Status registers" )
        for n in sorted(lStatDump):
            echo( "  {} {}".format(n, hex(lStatDump[n])))
        echo()

        lTimeStamp = lTStampNode.readBlock(2)
        lEventCtr = lPartNode.getNode('evtctr').read()
        lBufCount = lPartNode.getNode('buf.count').read()
        lPartNode.getClient().dispatch()

        lTime = int(lTimeStamp[0]) + (int(lTimeStamp[1]) << 32)
        echo( "Timestamp: {} ({})".format(style(str(lTime), fg='blue'), hex(lTime)) )
        echo( "EventCounter: {}".format(lEventCtr))
        lBufState = style('OK', fg='green') if lStatDump['buf_err'] == 0 else style('Error', fg='red')
        # lStatDump['buf_empty']
        echo( "Buffer status: " + lBufState)
        echo( "Buffer occupancy: {}".format(lBufCount))

        echo()
        toolbox.printCounters( lPartNode, {
            'actrs': 'Accept counters',
            'rctrs': 'Reject counters',
            })

        if watch:
            time.sleep(period)
        else:
            break
# ------------------------------------------------------------------------------

# -----------------
def read_mask(ctx, param, value):
    return int(value, 16)
# -----------------

# ------------------------------------------------------------------------------
@partition.command('configure', short_help='Prepares partition for data taking.')
@click.option('--trgmask', '-m', type=str, callback=lambda c, p, v: int(v, 16), default='0xf', help='Trigger mask (in hex).')
@click.pass_obj
def configure(obj, trgmask):
    '''
    Configures partition for data taking

    \b
    - disable command generator (calibration)
    - enable time-sync command generator
    - disable readout buffer
    - disable triggers
    - set command mask for the partition
    - enable partition
    \b
    Note: The trigger mask does not cover fake triggers which mask is automatically
    set according to the partition number.
    '''
    lPartId = obj.mPartitionId
    lPartNode = obj.mPartitionNode

    lFakeMask = (0x1 << lPartId)
    lTrgMask = (trgmask << 4) | lFakeMask;

    echo()
    echo("Configuring partition {}".format(lPartId))
    echo("Trigger mask set to {}".format(hex(lTrgMask)))
    echo("  Fake mask {}".format(hex(lFakeMask)))
    echo("  Phys mask {}".format(hex(trgmask)))

    lPartNode.reset(); 
    lPartNode.writeTriggerMask(lTrgMask);
    lPartNode.enable();
    secho("Partition {} enabled".format(lPartId), fg='green')

# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
@partition.command('start', short_help='Starts data taking.')
@click.pass_obj
def start(obj):

    '''
    Starts a new run
    
    \b
    - flushes the partition buffer
    - set the command mask
    - enables the readout buffer
    - enables triggers
    '''
    obj.mPartitionNode.start()
    secho("Partition {} started".format(obj.mPartitionId), fg='green')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@partition.command('trig', short_help='Toggles triggers')
@click.option('--on/--off', default=True, help='enable/disable triggers')
@click.pass_obj
def trig(obj, on):
    '''
    Toggles triggers.
    '''
    obj.mPartitionNode.enableTriggers(on)
    secho("Partition {} triggers {}".format(obj.mPartitionId, 'enabled' if on else 'disbaled'), fg='green')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@partition.command('stop', short_help='Stops data taking.')
@click.pass_obj
def stop(obj):
    
    '''
    Stop the run
    
    \b
    - disables triggers
    - disables the readout buffer
    '''

    # Select the desired partition
    obj.mPartitionNode.stop()
    secho("Partition {} stopped".format(obj.mPartitionId), fg='green')

# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
    # d(0) <= X"aa000600"; -- DAQ word 0
    # d(1) <= X"0000000" & scmd; -- DAQ word 1
    # d(2) <= tstamp(31 downto 0); -- DAQ word 2
    # d(3) <= tstamp(63 downto 32); -- DAQ word 3
    # d(4) <= evtctr; -- DAQ word 4
    # d(5) <= X"00000000"; -- Dummy checksum (not implemented yet)
kEventSize = 6

@partition.command('readback', short_help='Read the timing master readout buffer.')
@click.pass_obj
@click.option('--events/--all', ' /-a', 'readall', default=False, help="Buffer readout mode.\n- events: only completed events are readout.\n- all: the content of the buffer is fully read-out.")
@click.option('--keep-reading', '-k', 'keep', is_flag=True, default=False, help='Continuous buffer readout')
def readback(obj, readall, keep):
    '''
    Read the content of the timing master readout buffer.
    '''
    # lDevice = obj.mDevice
    lPartId = obj.mPartitionId
    lPartNode = obj.mPartitionNode

    while(True):
        lBufCount = lPartNode.getNode('buf.count').read()
        lPartNode.getClient().dispatch()

        echo ( "Words available in readout buffer: "+hex(lBufCount))
        
        lWordsToRead = int(lBufCount) if readall else (int(lBufCount) / kEventSize)*kEventSize

        # if lWordsToRead == 0:
            # echo("Nothing to read, goodbye!")

        lBufData = lPartNode.getNode('buf.data').readBlock(lWordsToRead)
        lPartNode.getClient().dispatch()

        for i, lWord in enumerate(lBufData):
            echo ( '{:04d} {}'.format(i, hex(lWord)))
        if not keep:
            break
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@master.command('send', short_help='Inject a single command.')
@click.pass_obj
@click.argument('cmd', type=click.Choice(defs.kCommandIDs.keys()))
@click.option('-n', type=int, default=1)
def send(obj, cmd, n):
    '''
    Inject a single command.

    CMD (str): Name of the command to inject '''
    # + ','.join(defs.kCommandIDs.keys())

    # lDevice = obj.mDevice
    lMaster = obj.mMaster

    lGenChanCtrl = lMaster.getNode('scmd_gen.chan_ctrl')

    toolbox.resetSubNodes(lGenChanCtrl)

    for i in xrange(n):
        lGenChanCtrl.getNode('type').write(defs.kCommandIDs[cmd])
        lGenChanCtrl.getNode('force').write(0x1)
        lTStamp = lMaster.getNode("tstamp.ctr").readBlock(2)
        lMaster.getClient().dispatch()

        lGenChanCtrl.getNode('force').write(0x0)
        lMaster.getClient().dispatch()
        lTimeStamp = int(lTStamp[0]) + (int(lTStamp[1]) << 32)
        echo("Command sent {}({}) @time {} {}".format(style(cmd, fg='blue'), defs.kCommandIDs[cmd], hex(lTimeStamp), lTimeStamp))
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------

# -----------------
def validate_freq(ctx, param, value):

    lFqMin = 0.5
    lFqMax = 12500.0

    if value < lFqMin or value > lFqMax:    
        raise click.BadParameter(
            'Frequency out of the allowed range {}-{} Hz'.format(lFqMin, lFqMax)
            )

    def div2freq(div):
        return 50e6/(1<<(12+div))

    # Brute force it
    lDeltas = [ abs(value-div2freq(div)) for div in xrange(0x10) ]
    lMinDeltaIdx = min(xrange(len(lDeltas)), key=lDeltas.__getitem__)
    return value, lMinDeltaIdx, div2freq(lMinDeltaIdx)
# -----------------

@master.command('faketrig-conf')
@click.pass_obj
@click.argument('chan', type=int, callback=toolbox.validate_chan)
# @click.argument('divider', type=click.IntRange(0, 0x3ff))
@click.argument('rate', type=float)
@click.option('--poisson', is_flag=True, default=False, help="Randomize time interval between consecutive triggers.")
def faketriggen(obj, chan, rate, poisson):
    '''
    \b
    Enables the internal trigger generator.
    Configures the internal command generator to produce triggers at a defined frequency.
    
    Rate =  (50MHz / 2^(d+8)) / p where n in [0,15] and p in [1,256]
    \b
    DIVIDER (int): Frequency divider.
    '''

    lClockRate = 50e6
    # The division from 50MHz to the desired rate is done in three steps:
    # a) A pre-division by 256
    # b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
    # c) 1-in-n prescaling set by n = rate_div_p
    lDiv = int(math.ceil(math.log(lClockRate / (rate * 256 * 256), 2)))
    if lDiv < 0:
        lDiv = 0
    elif lDiv > 15:
        lDiv = 15
    lPS = int(lClockRate / (rate * 256 * (1 << lDiv)) + 0.5)
    if lPS == 0 or lPS > 256:
        raise click.Exception("Req rate is out of range")
    else:
        a = lClockRate / float(256 * lPS * (1 << lDiv))
        secho( "Requested rate, actual rate: {} {}".format(rate, a), fg='cyan' )
        secho( "prescale, divisor: {} {}".format(lPS, lDiv), fg='cyan')


    lMaster = obj.mMaster
    lGenChanCtrl = lMaster.getNode('scmd_gen.chan_ctrl')

    kFakeTrigID = 'FakeTrig{}'.format(chan)

    lMaster.getNode('scmd_gen.sel').write(chan)

    lGenChanCtrl.getNode('type').write(defs.kCommandIDs[kFakeTrigID])
    lGenChanCtrl.getNode('rate_div_d').write(lDiv)
    lGenChanCtrl.getNode('rate_div_p').write(lPS)
    lGenChanCtrl.getNode('patt').write(poisson)
    lGenChanCtrl.getClient().dispatch()
    echo( "> Trigger rate for {} ({}) set to {}".format( 
        kFakeTrigID,
        hex(defs.kCommandIDs[kFakeTrigID]),
        style(
            "{:.3e} Hz".format( a ),
            fg='yellow'
            )
        )
    )
    echo( "> Trigger mode: " + style({False: 'periodic', True: 'poisson'}[poisson], fg='cyan') )

    lGenChanCtrl.getNode("en").write(1) # Start the command stream
    lGenChanCtrl.getClient().dispatch()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@master.command('faketrig-clear')
@click.pass_obj
@click.argument('chan', type=int, callback=toolbox.validate_chan)
def faketrigclear(obj, chan):
    '''
    Clear the internal trigger generator.
    '''
    lMaster = obj.mMaster

    lGenChanCtrl = lMaster.getNode('scmd_gen.chan_ctrl')
    lMaster.getNode('scmd_gen.sel').write(chan)

    toolbox.resetSubNodes(lGenChanCtrl)
    secho( "> Fake trigger generator {} configuration cleared".format(chan), fg='cyan' )

# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
# -- cyc_len and spill_len are in units of 1 / (50MHz / 2^24) = 0.34s
@master.command()
@click.pass_obj
def spillgen(obj):
    '''
    \b
    Enables the internal spill generator.
    Configures the internal command generator to produce spills at a defined frequency and length
    
    Rate = 50 Mhz / 2**( 12 + divider ) for divider between 0 and 15
# -- cyc_len and spill_len are in units of 1 / (50MHz / 2^24) = 0.34s

    \b
    FREQ
    '''
    lMaster = obj.mMaster

    lSpillCtrl = lMaster.getNode('spill.csr.ctrl')
    lSpillCtrl.getNode('fake_cyc_len').write(16)
    lSpillCtrl.getNode('fake_spill_len').write(8)
    lSpillCtrl.getNode('en_fake').write(1)
    lSpillCtrl.getClient().dispatch()
# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
@master.command('sfp', short_help='Query the sfp parameters.')
@click.pass_obj
def sfp(obj):
    '''
    Read the content of the timing master readout buffer.
    '''
    lDevice = obj.mDevice

    lSfp = lDevice.getNode('io.sfp_i2c')
    print ( lSfp )
    print ( lSfp.getSlave('eeprom') )
    print ( hex(lSfp.getSlave('eeprom').getI2CAddress()) )
    # print ( lSfp.getSlave('eeprom').readI2C(0x0) )

    try:
        import IPython
    except ImportError:
        echo('Failed to load IPython')
        return

    echo('Starting IPython - {} connected to variable \'board\''.format( lDevice.id() ))
    IPython.embed()

# ------------------------------------------------------------------------------
