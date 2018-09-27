from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import math
import pdt
import traceback
import StringIO

# PDT imports
import pdt.cli.toolbox as toolbox
import pdt.cli.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars, basename
from pdt.core import SI534xSlave, I2CExpanderSlave

kMasterFWMajorRequired = 5


from pdt.cli.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from pdt.cli.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from pdt.cli.definitions import kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout
from pdt.cli.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap
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
    echo("ID: design '{}' on board '{}' on carrier '{}'".format(
        style(kDesignNameMap[lBoardInfo['design_type'].value()], fg='blue'),
        style(kBoardNamelMap[lBoardInfo['board_type'].value()], fg='blue'),
        style(kCarrierNamelMap[lBoardInfo['carrier_type'].value()], fg='blue')
    ))
    echo("Master FW rev: {}, partitions: {}, channels: {}".format(
        style(hex(lVersion), fg='cyan'),
        style(str(lGenerics['n_part']), fg='cyan'),
        style(str(lGenerics['n_chan']), fg='cyan'),
    ))

    if lMajor < 4:
        secho('ERROR: Incompatible master firmware version. Found {}, required {}'.format(hex(lMajor), hex(kMasterFWMajorRequired)), fg='red')
        raise click.Abort()

    obj.mDevice = lDevice
    obj.mMaster = lMaster
    obj.mExtTrig = lDevice.getNode('master_top.trig')
    
    obj.mGenerics = { k:v.value() for k,v in lGenerics.iteritems()}
    obj.mVersion = lVersion.value()
    obj.mBoardType = lBoardInfo['board_type'].value()
    obj.mCarrierType = lBoardInfo['carrier_type'].value()
    obj.mDesignType = lBoardInfo['design_type'].value()
    
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------

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
    kPC059FanoutSFP: "devel/PDTS_PC059_FANOUT_SFP_IN.txt",
    # kTLURev1: "devel/PDTS_TLU_MASTER.txt"
    kTLURev1: "devel/PDTS_TLU_MASTER_ONLYLEMOIN.txt"
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
    0x5410ec6476f1: kFMCRev2,
    0xd88039d980cf: kPC059Rev1,
    0xd88039d98adf: kPC059Rev1,
    0xd88039d92491: kPC059Rev1,
    0xd88039d9248e: kPC059Rev1,
    0xd88039d98ae9: kPC059Rev1,
    0xd88039d92498: kPC059Rev1,
}


# ------------------------------------------------------------------------------
@master.command('fixtime', short_help="Measure some frequencies.")
@click.pass_obj
def fixtime(obj):

    lMaster = obj.mMaster
    lTStampNode = lMaster.getNode('tstamp.ctr')

    lTimeStamp = lTStampNode.readBlock(2)
    lTStampNode.getClient().dispatch()
    lTime = int(lTimeStamp[0]) + (int(lTimeStamp[1]) << 32)
    secho('Old Timestamp {}'.format(hex(lTime)), fg='cyan')

    # Setting timestamp
    lNow = int(time.time()*50e6)
    lNowH = (lNow >> 32) & ((1<<32)-1)
    lNowL = (lNow >> 0) & ((1<<32)-1)

    lTStampNode.writeBlock([lNowH, lNowL])
    lTStampNode.getClient().dispatch()

    lTimeStamp = lTStampNode.readBlock(2)
    lTStampNode.getClient().dispatch()

    lTime = int(lTimeStamp[0]) + (int(lTimeStamp[1]) << 32)
    secho('New Timestamp {}'.format(hex(lTime)), fg='cyan')

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
@partition.command('status', short_help='Display the status of the timing master.')
@click.pass_obj
@click.option('--watch', '-w', is_flag=True, default=False, help='Turn on automatic refresh')
@click.option('--period','-p', type=click.IntRange(0, 10), default=2, help='Automatic refresh period')
def partstatus(obj, watch, period):
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

        # echo()
        # secho( "=> Cmd generator control", fg='cyan')

        # lScmdGenChanCtrlDump = toolbox.readSubNodes(lScmdGenNode.getNode('chan_ctrl'))
        # toolbox.printRegTable(lScmdGenChanCtrlDump, False)

        # echo()
        secho( "=> Cmd generator counters", fg='cyan')
        toolbox.printCounters( lScmdGenNode, {
            'actrs': 'Accept counters',
            'rctrs': 'Reject counters',
            }, lNumChan, 'Chan', {})

        # secho( "=> Spill generator control", fg='green')
        # lDump = toolbox.readSubNodes(lDevice.getNode('master.spill.csr.ctrl'))
        # for n in sorted(lDump):
        #     echo( "  {} {}".format(n, hex(lDump[n])))
        # echo()
        # secho( "=> Spill generator stats", fg='green')
        # lDump = toolbox.readSubNodes(lDevice.getNode('master.spill.csr.stat'))
        # for n in sorted(lDump):
        #     echo( "  {} {}".format(n, hex(lDump[n])))
        echo()

        secho( "=> Partition {}".format(lPartId), fg='green')

        lCtrlDump = toolbox.readSubNodes(lPartNode.getNode('csr.ctrl'))
        lStatDump = toolbox.readSubNodes(lPartNode.getNode('csr.stat'))

        # echo()
        # secho("Control registers", fg='cyan')
        # toolbox.printRegTable(lCtrlDump, False)
        # echo()
        # secho("Status registers", fg='cyan')
        # toolbox.printRegTable(lStatDump, False)
        # echo()

        #-------------- Temp
        ctrls = toolbox.formatRegTable(lCtrlDump, False).split('\n')
        ctrls.insert(0, style("Control ", fg="cyan"))
        stats = toolbox.formatRegTable(lStatDump, False).split('\n')
        stats.insert(0, style("Status registers", fg='cyan'))

        col1 = max([len(toolbox.escape_ansi(l)) for l in ctrls])
        col2 = max([len(toolbox.escape_ansi(l)) for l in stats])

        nrows = max(len(ctrls), len(stats));

        ctrls += [''] * (nrows - len(ctrls))
        stats += [''] * (nrows - len(stats))
        fmt = '\'{:<%d}\' \'{:<%d}\'' % (col1, col2)
        for c,s in zip(ctrls, stats):
            print (c + ' '*(col1-len(toolbox.escape_ansi(c))), '' ,s + ' '*(col2-len(toolbox.escape_ansi(s))))
        #-------------- Temp
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
@click.option('--spill-gate/--no-spill-gate', 'spillgate', default=True, help='Enable the spill gate')
@click.pass_obj
def configure(obj, trgmask, spillgate):
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
    lPartNode.configure(lTrgMask, spillgate);
    lPartNode.enable();
    secho("Partition {} enabled and configured".format(lPartId), fg='green')

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
    def chunks(l, n):
        """Yield successive n-sized chunks from l."""
        for i in range(0, len(l), n):
            yield l[i:i + n]

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



        for i,c in enumerate(chunks(lBufData, 6)):
            ts = (c[3]<<32) +c[2]
            # print ('header:', hex(c[0]), hex(c[1]))
            print ('ev {} - ts    : {} ({})'.format(i, ts, hex(ts)))


        # for i, lWord in enumerate(lBufData):
            # echo ( '{:04d} {}'.format(i, hex(lWord)))
        if not keep:
            break
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@master.command('send-cmd', short_help='Inject a single command.')
@click.pass_obj
@click.argument('cmd', type=click.Choice(defs.kCommandIDs.keys()))
@click.argument('chan', type=int, callback=toolbox.validate_chan)
@click.option('-n', type=int, default=1)
def sendcmd(obj, cmd, chan, n):
    '''
    Inject a single command.

    CMD (str): Name of the command to inject '''
    # + ','.join(defs.kCommandIDs.keys())

    # lDevice = obj.mDevice
    lMaster = obj.mMaster
    lMaster.getNode('scmd_gen.sel').write(chan)
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
        echo("Command sent {}({}) from generator {} @time {} {}".format(style(cmd, fg='blue'), defs.kCommandIDs[cmd], chan, hex(lTimeStamp), lTimeStamp))
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
@master.command('fake-spillgen')
@click.pass_obj
def fake_spillgen(obj):
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
    lSpillCtrl.getNode('src').write(1)
    lSpillCtrl.getNode('en').write(1)
    lSpillCtrl.getClient().dispatch()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@master.group('align')
@click.pass_obj
def align(obj):
    lMaster = obj.mMaster
    obj.mGlobal = lMaster.getNode('global')
    obj.mACmd = lMaster.getNode('acmd')
    obj.mEcho = lMaster.getNode('echo')
# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
def enableEptAndWaitForReady( aGlobal, aTimeout=2 ):
    aGlobal.getNode('csr.ctrl.ep_en').write(0x0)
    aGlobal.getNode('csr.ctrl.ep_en').write(0x1)
    aGlobal.getClient().dispatch()

    # Wait for the endpoint to be happy
    lTOutStart = time.time()
    while time.time() < lTOutStart + aTimeout:
        time.sleep(0.1)

        lEptStat = aGlobal.getNode('csr.stat.ep_stat').read()
        lEptRdy = aGlobal.getNode('csr.stat.ep_rdy').read()
        aGlobal.getClient().dispatch()

        print ('stat', hex(lEptStat))
        print ('rdy ', hex(lEptRdy))
        if int(lEptRdy) == 1:
            break
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def sendEchoAndMeasureDelay( aEcho, aTimeout=2 ):
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
        raise RuntimeError("aaaa")

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
def pushDelay( aACmd, aAddr, aCDel):
    # Keep the TX sfp on
    toolbox.resetSubNodes(aACmd.getNode('csr.ctrl'), dispatch=False)
    aACmd.getNode('csr.ctrl.tx_en').write(0x1)
    aACmd.getNode('csr.ctrl.addr').write(aAddr)
    aACmd.getNode('csr.ctrl.cdel').write(aCDel)
    aACmd.getNode('csr.ctrl.update').write(0x1)
    aACmd.getNode('csr.ctrl.go').write(0x1)
    aACmd.getNode('csr.ctrl.go').write(0x0)
    aACmd.getClient().dispatch()
    secho('Coarse delay {} applied'.format(aCDel), fg='green')
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
@click.option('--mux', '-m', type=click.IntRange(0,7), help='Mux select (fanout only)')
@click.option('--force', '-f', is_flag=True, default=False, help='Mux select (fanout only)')
@click.pass_obj
@click.pass_context
def align_applydelay(ctx, obj, addr, delay, mux, force):

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
            enableEndpointSFP(lACmd, 0x0, False)
            # Turn on the current target
            enableEndpointSFP(lACmd, addr)

            time.sleep(0.1)

            enableEptAndWaitForReady(lGlobal)

            lTimeTx, lTimeRx = sendEchoAndMeasureDelay(lEcho)

            print(lTimeTx, lTimeRx, lTimeRx-lTimeTx)
            #------------

            lGlobal.getNode('csr.ctrl.ep_en').write(0x0)

        pushDelay(lACmd, addr, delay)

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
def align_measuredelay(ctx, obj, addr, mux):

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

    # Switch off all TX SFPs
    enableEndpointSFP(lACmd, 0x0, False)
    # Turn on the current target
    enableEndpointSFP(lACmd, addr)

    time.sleep(0.1)

    enableEptAndWaitForReady(lGlobal)

    lTimeTx, lTimeRx = sendEchoAndMeasureDelay(lEcho)
    print(lTimeTx, lTimeRx, lTimeRx-lTimeTx)

    enableEndpointSFP(lACmd, addr, 0x0)

# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@align.command('toggle-tx', short_help="Control the trigger return endpoint")
@click.argument('addr', type=toolbox.IntRange(0x0,0x100))
@click.option('--on/--off', default=True, help='enable/disable _ALL_ enpoint tx')
@click.pass_obj
def align_toggletx(obj, addr, on):

    lACmd = obj.mACmd

    toolbox.resetSubNodes(lACmd.getNode('csr.ctrl'))

    lACmd.getNode('csr.ctrl.addr').write(addr)
    lACmd.getNode('csr.ctrl.tx_en').write(on)
    lACmd.getNode('csr.ctrl.go').write(0x1)
    lACmd.getNode('csr.ctrl.go').write(0x0)
    lACmd.getClient().dispatch()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
# -- cyc_len and spill_len are in units of 1 / (50MHz / 2^24) = 0.34s
@master.group('ext-trig')
@click.pass_obj
def externaltrigger(obj):
    pass
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@externaltrigger.command('delay', short_help="Control the trigger return endpoint")
@click.argument('action', default='enable', type=click.Choice(['enable', 'disable', 'reset']))
@click.pass_obj
@click.pass_context
def exttrg_ept(ctx, obj, action):

    lExtTrig = obj.mExtTrig

    # Disable comes first
    if action in ['disable','reset']:
        lExtTrig.getNode('csr.ctrl.ep_en').write(0x0)
    # And then enable
    if action in ['enable','reset']:
        lExtTrig.getNode('csr.ctrl.ep_en').write(0x1)

    lExtTrig.getClient().dispatch()
    secho("Trigger endpoint action '" + action + "' completed", fg='green')

    ctx.invoke(exttrgmonitor)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@externaltrigger.command('enable', short_help="Enable external triggers")
@click.option('--on/--off', default=True, help='enable/disable triggers')
@click.pass_obj
@click.pass_context
def exttrg_enable(ctx, obj, on):

    lExtTrig = obj.mExtTrig

    lExtTrig.getNode('csr.ctrl.ext_trig_en').write(on)
    lExtTrig.getClient().dispatch()
    secho("External triggers " + ("enabled" if on else "disabled"), fg='green')
    ctx.invoke(exttrgmonitor)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@externaltrigger.command('monitor', short_help="Monitor trigger input status")
@click.option('--watch', '-w', is_flag=True, default=False, help='Turn on automatic refresh')
@click.option('--period','-p', type=click.IntRange(0, 10), default=2, help='Automatic refresh period')
@click.pass_obj
def exttrg_monitor(obj, watch, period):
    
    lExtTrig = obj.mExtTrig


    while(True):
        if watch:
            click.clear()
        
        echo()

        secho('External trigger endpoint', fg='blue')
        echo()
        
        lCtrlDump = toolbox.readSubNodes(lExtTrig.getNode('csr.ctrl'))
        lStatDump = toolbox.readSubNodes(lExtTrig.getNode('csr.stat'))

        echo( "Endpoint State: {}".format(style(defs.fmtEpState(lStatDump['ep_stat']), fg='blue')))
        echo()
        secho("Control registers", fg='cyan')
        toolbox.printRegTable(lCtrlDump, False)
        echo()
        secho("Status registers", fg='cyan')
        toolbox.printRegTable(lStatDump, False)
        echo()

        toolbox.printCounters( lExtTrig, {
            'ctrs': 'Counters',
        })
        if watch:
            time.sleep(period)
        else:
            break
# ------------------------------------------------------------------------------

