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
import pdt.common.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars, basename
from pdt.core import SI534xSlave, I2CExpanderSlave

kMasterFWMajorRequired = 5


from pdt.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from pdt.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from pdt.common.definitions import kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout
from pdt.common.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap
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

    if lMajor < kMasterFWMajorRequired:
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
@master.command('synctime', short_help="Sync timestamps with computer local time.")
@click.pass_obj
def synctime(obj):

    lMaster = obj.mMaster
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

    print (toolbox.formatTStamp(lTimeStamp))

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

    lTStampNode = lMaster.getNode('tstamp.ctr.val')

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

        lTime = (int(lTimeStamp[1]) << 32) + int(lTimeStamp[0])
        echo( "Timestamp: {} -> {}".format(style(hex(lTime), fg='blue'), toolbox.formatTStamp(lTimeStamp)))
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

    # lPartId = obj.mPartitionId
    lPartNode = obj.mPartitionNode

    while(True):
        lBufCount = lPartNode.getNode('buf.count').read()
        lPartNode.getClient().dispatch()

        echo ( "Words available in readout buffer: "+hex(lBufCount))

        lWordsToRead = int(lBufCount) if readall else (int(lBufCount) / defs.kEventSize)*defs.kEventSize

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
@partition.command('rate-ctrl', short_help='Enable/Disable rate control.')
@click.pass_obj
@click.option('-e/-d', '--enable/--disable', 'rate_ctrl_en', default=True, help="Toggle rate control register.")
def rate_ctrl(obj, rate_ctrl_en):
    lPartId = obj.mPartitionId
    lPartNode = obj.mPartitionNode

    lPartNode.getNode('csr.ctrl.rate_ctrl_en').write(rate_ctrl_en);
    lPartNode.getClient().dispatch()

    lVal = lPartNode.getNode('csr.ctrl.rate_ctrl_en').read()
    lPartNode.getClient().dispatch()

    echo("Trigger throttling in partition {}: {}".format(lPartId, 'Enabled' if bool(lVal) else 'Disabled'))


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
        lTStamp = lMaster.getNode("tstamp.ctr.val").readBlock(2)
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
    secho( "Fake trigger generator {} configuration cleared".format(chan), fg='green' )

# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
# -- cyc_len and spill_len are in units of 1 / (50MHz / 2^24) = 0.34s
@master.command('spill-enable')
@click.pass_obj
def spillenable(obj):
    '''
    \b
    Enables the internal spill generator.
    '''
    lMaster = obj.mMaster

    lSpillCtrl = lMaster.getNode('spill.csr.ctrl')
    lSpillCtrl.getNode('src').write(0)
    lSpillCtrl.getNode('en').write(1)
    lSpillCtrl.getClient().dispatch()
    secho( "Spill interface enabled", fg='green' )

# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
# -- cyc_len and spill_len are in units of 1 / (50MHz / 2^24) = 0.34s
@master.command('fake-spillgen')
@click.pass_obj
def fakespillgen(obj):
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

