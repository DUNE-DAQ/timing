from __future__ import print_function

# Python imports
import uhal
import click
import click_didyoumean
import time
import collections
import math
import timing
import traceback
from io import StringIO

# PDT imports
import timing.cli.toolbox as toolbox
import timing.common.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars, basename
from timing.core import SI534xSlave, I2CExpanderSlave

kMasterFWMajorRequired = 5


from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.common.definitions import kDesingMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesingFanout
from timing.common.definitions import kBoardNamelMap, kCarrierNamelMap, kDesignNameMap
from timing.common.definitions import kLibrarySupportedBoards
# ------------------------------------------------------------------------------
#    __  ___         __         
#   /  |/  /__ ____ / /____ ____
#  / /|_/ / _ `(_-</ __/ -_) __/
# /_/  /_/\_,_/___/\__/\__/_/   
                        

@click.group('mst', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, autocompletion=toolbox.completeDevices)
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

    # print({ k:v.value() for k,v in lBoardInfo.items()})
    # raise SystemExit(0)

    lMajor = (lVersion >> 16) & 0xff
    lMinor = (lVersion >> 8) & 0xff
    lPatch = (lVersion >> 0) & 0xff
    
    if lBoardInfo['board_type'].value() in kLibrarySupportedBoards:
        echo(lDevice.getNode('io').get_hardware_info())

    echo("Master FW rev: {}, partitions: {}, channels: {}".format(
        style(hex(lVersion), fg='cyan'),
        style(str(lGenerics['n_part']), fg='cyan'),
        style(str(lGenerics['n_chan']), fg='cyan'),
    ))

    if lMajor < kMasterFWMajorRequired:
        secho('ERROR: Incompatible master firmware version. Found {}, required {}'.format(hex(lMajor), hex(kMasterFWMajorRequired)), fg='red')
        raise click.Abort()

    obj.mDevice = lDevice
    obj.mTopDesign = lDevice.getNode('')
    obj.mMaster = lMaster
    obj.mMasterTop = lDevice.getNode('master_top')
    obj.mExtTrig = lDevice.getNode('master_top.trig')
    
    obj.mGenerics = { k:v.value() for k,v in lGenerics.items()}
    obj.mVersion = lVersion.value()
    obj.mBoardType = lBoardInfo['board_type'].value()
    obj.mCarrierType = lBoardInfo['carrier_type'].value()
    obj.mDesignType = lBoardInfo['design_type'].value()
    
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@master.command('status', short_help="Print master status")
@click.pass_obj
def synctime(obj):

    lMasterTop = obj.mMasterTop
    echo(lMasterTop.get_status())
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@master.command('synctime', short_help="Sync timestamps with computer local time.")
@click.pass_obj
def synctime(obj):

    lMasterTop = obj.mMasterTop
    lMasterTop.sync_timestamp()
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
    lMasterTop = obj.mMasterTop
    lPartNode = obj.mPartitionNode

    lTStampNode = lMaster.getNode('tstamp.ctr.val')

    while(True):
        if watch:
            click.clear()
        echo()
        echo( "-- " + style("Master state", fg='green') + "---")
        echo()
        
        echo(lMasterTop.get_status())

        echo()

        echo(lPartNode.get_status())

        if watch:
            time.sleep(period)
        else:
            break
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@partition.command('configure', short_help='Prepares partition for data taking.')
@click.option('--trgmask', '-m', type=str, callback=lambda c, p, v: int(v, 16), default='0xf', help='Trigger mask (in hex).')
@click.option('--spill-gate/--no-spill-gate', 'spillgate', default=True, help='Enable the spill gate')
@click.option('--rate-ctrl/--no-rate-ctrl', 'ratectrl', default=True, help='Enable rate control')
@click.pass_obj
def configure(obj, trgmask, spillgate, ratectrl):
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
    lPartNode.configure(lTrgMask, spillgate, ratectrl);
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
    obj.mPartitionNode.enable_triggers(on)
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
        lBufCount = lPartNode.read_buffer_word_count()

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

    lPartNode.configure_rate_ctrl(rate_ctrl_en)
    echo("Trigger throttling in partition {}: {}".format(lPartId, 'Enabled' if bool(rate_ctrl_en) else 'Disabled'))
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
    '''

    lMasterTop = obj.mMasterTop
    lMasterTop.send_fl_cmd(defs.kCommandIDs[cmd],chan,n)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
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
    lDeltas = [ abs(value-div2freq(div)) for div in range(0x10) ]
    lMinDeltaIdx = min(range(len(lDeltas)), key=lDeltas.__getitem__)
    return value, lMinDeltaIdx, div2freq(lMinDeltaIdx)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
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

    # The division from 50MHz to the desired rate is done in three steps:
    # a) A pre-division by 256
    # b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
    # c) 1-in-n prescaling set by n = rate_div_p

    lMasterTop = obj.mMasterTop
    lMasterTop.enable_fake_trigger(chan,rate,poisson)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@master.command('faketrig-clear')
@click.pass_obj
@click.argument('chan', type=int, callback=toolbox.validate_chan)
def faketrigclear(obj, chan):
    '''
    Clear the internal trigger generator.
    '''
    lMasterTop = obj.mMasterTop
    lMasterTop.disable_fake_trigger(chan)
    secho( "Fake triggers disabled; chan: {}".format(chan), fg='green')
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
    lMasterTop = obj.mMasterTop
    lMasterTop.enable_spill_interface()
    secho( "Spill interface enabled", fg='green')
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
    lMasterTop = obj.mMasterTop
    lMasterTop.enable_fake_spills()
    secho( "Fake spills enabled", fg='green')

# ------------------------------------------------------------------------------