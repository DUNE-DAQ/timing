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
import sys
import random
from io import StringIO

# PDT imports
import timing.cli.toolbox as toolbox
import timing.common.definitions as defs

from click import echo, style, secho
from os.path import join, expandvars, basename
from timing.core import SI534xSlave, I2CExpanderSlave

from timing.common.definitions import kBoardSim, kBoardFMC, kBoardPC059, kBoardMicrozed, kBoardTLU
from timing.common.definitions import kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignOverlord
from timing.common.definitions import kBoardNameMap, kCarrierNameMap, kDesignNameMap
from timing.common.definitions import kLibrarySupportedBoards, kLibrarySupportedDesigns

from timing.common.toolbox import format_firmware_version
# ------------------------------------------------------------------------------
#    __  ___         __         
#   /  |/  /__ ____ / /____ ____
#  / /|_/ / _ `(_-</ __/ -_) __/
# /_/  /_/\_,_/___/\__/\__/_/   
                        

@click.group('mst', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, shell_complete=toolbox.completeDevices)
def master(obj, device):
    '''
    Timing master commands.

    DEVICE: uhal device identifier
    '''
    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)
        
    echo('Created device ' + click.style(lDevice.id(), fg='blue'))

    lTopDesign = lDevice.getNode('')
    
    lMaster = lDevice.getNode('master')    
    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lNCmdchannels = lMaster.getNode('global.config.n_chan').read()
    lDevice.dispatch()

    echo("Design '{}' on board '{}' on carrier '{}' with frequency {} MHz".format(
        style(kDesignNameMap[lBoardInfo['design_type'].value()], fg='blue'),
        style(kBoardNameMap[lBoardInfo['board_type'].value()], fg='blue'),
        style(kCarrierNameMap[lBoardInfo['carrier_type'].value()], fg='blue'),
        style(str(lBoardInfo['clock_frequency'].value()/1e6), fg='blue')
    ))

    if lBoardInfo['board_type'].value() in kLibrarySupportedBoards and lBoardInfo['design_type'].value() in kLibrarySupportedDesigns:
        lVersion = lTopDesign.read_firmware_version()
        lTopDesign.validate_firmware_version()

        try:
            echo(lDevice.getNode('io').get_hardware_info())
        except:
            secho("Failed to retrieve hardware information! I2C issue? Initial board reset needed?", fg='yellow')
            e = sys.exc_info()[0]
            secho("Error: {}".format(e), fg='red')
    else:
        lVersion = lMaster.getNode('global.version').read()
        lDevice.dispatch()

    echo("Master FW rev: {}, channels: {}".format(
        style(format_firmware_version(lVersion), fg='cyan'),
        style(str(lNCmdchannels), fg='cyan'),
    ))

    obj.mDevice = lDevice
    obj.mTopDesign = lTopDesign
    obj.mMaster = lMaster
    obj.mIO = lDevice.getNode('io')

    obj.mVersion = lVersion
    obj.mBoardType = lBoardInfo['board_type'].value()
    obj.mCarrierType = lBoardInfo['carrier_type'].value()
    obj.mDesignType = lBoardInfo['design_type'].value()

    # only overlord has ext trig ept
    if obj.mDesignType == kDesignOverlord:
        obj.mExtTrig = obj.mTopDesign.get_external_triggers_endpoint_node()
    
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@master.command('status', short_help="Print master status")
@click.pass_obj
def status(obj):
    
    lMaster = obj.mMaster
    lIO = obj.mIO
    firmware_clock_frequency_hz = lIO.read_firmware_frequency()

    echo(lMaster.get_status_with_date(firmware_clock_frequency_hz))
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@master.command('synctime', short_help="Sync timestamps with computer local time.")
@click.pass_obj
def synctime(obj):

    lDesign = obj.mTopDesign
    lDesign.sync_timestamp()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@master.command('send-cmd', short_help='Inject a single command.')
@click.pass_obj
@click.argument('cmd', type=toolbox.IntRange(0x0,0xff))
@click.argument('chan', type=int)
@click.option('-n', type=int, default=1)
def sendcmd(obj, cmd, chan, n):
    '''
    Inject a single command.
    '''

    lMaster = obj.mMaster
    lMaster.send_fl_cmd(cmd,chan,n)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@master.command('faketrig-conf')
@click.pass_obj
@click.argument('cmd', type=toolbox.IntRange(0x0,0xff))
@click.argument('chan', type=int)
@click.argument('rate', type=float)
@click.option('--poisson', is_flag=True, default=False, help="Randomize time interval between consecutive triggers.")
def faketriggen(obj, cmd, chan, rate, poisson):
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

    lTopDesign = obj.mTopDesign
    lTopDesign.enable_periodic_fl_cmd(cmd,chan,rate,poisson)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@master.command('faketrig-clear')
@click.pass_obj
@click.argument('chan', type=int)
def faketrigclear(obj, chan):
    '''
    Clear the internal trigger generator.
    '''
    lMaster = obj.mMaster
    lMaster.disable_periodic_fl_cmd(chan)
    secho( "Fake triggers disabled; chan: {}".format(chan), fg='green')
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@master.command('write-ept-reg')
@click.pass_obj
@click.argument('adr', type=toolbox.IntRange(0x0,0xffff))
@click.argument('reg', type=toolbox.IntRange(0x0,0x78))
@click.argument('data', callback=toolbox.split_ints)
@click.argument('mode', type=bool)
def writeeptreg(obj, adr, reg, data, mode):
    '''
    Write data from endpoint
    '''
    lMaster = obj.mMaster
    rx_data = lMaster.write_endpoint_data(adr, reg, data, mode)
    #secho( "Fake triggers disabled; chan: {}".format(chan), fg='green')
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@master.command('read-ept-reg')
@click.pass_obj
@click.argument('adr', type=toolbox.IntRange(0x0,0xffff))
@click.argument('reg', type=toolbox.IntRange(0x0,0x78))
@click.argument('length', type=int)
@click.argument('mode', type=bool)
def readeptreg(obj, adr, reg, length, mode):
    '''
    Read data from endpoint
    '''
    lMaster = obj.mMaster
    rx_data = lMaster.read_endpoint_data(adr, reg, length, mode)
    secho( f"Data from endpoint {adr}, reg {reg} and length {length}: {rx_data}", fg='green')
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@master.command('transmit-async-packet')
@click.pass_obj
@click.option('--packet', '-p', callback=toolbox.split_ints)
def transmitasyncpacket(obj, packet):
    '''
    Transmit already formed packet
    '''
    lMaster = obj.mMaster
    if len(packet) == 0:
        # make a random packet
        packet = [0x0, 0x0] + random.sample(range(256),random.randint(5, 20)) + [0x1aa]
    
    rx_packet = lMaster.transmit_async_packet(packet)
    secho( f"tx packet: {packet}", fg='green')
    secho( f"rx packet: {rx_packet}", fg='green')
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@master.command('reset-command-counters')
@click.pass_obj
def resetcommandcounters(obj):
    '''
    Transmit already formed packet
    '''
    lMaster = obj.mMaster
    lMaster.reset_command_counters()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@master.command('control-timestamp-broadcast')
@click.option('--on/--off', default=True, help='enable/disable ts broadcast')
@click.pass_obj
def controltimestampbroadcast(obj, on):
    '''
    Enable or disable timestamp broadcast
    '''
    lMaster = obj.mMaster
    if on:
        lMaster.enable_timestamp_broadcast()
    else:
        lMaster.disable_timestamp_broadcast()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@master.group('part', invoke_without_command=True)
@click.pass_obj
def partition(obj):
    """
    Partition specific commands
    """
    secho("New partition concept not yet supported", fg='yellow')
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
    pass
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@partition.command('configure', short_help='Prepares partition for data taking.')
@click.pass_obj
def configure(obj):
    pass
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@master.command('configure-endpoint-cmd-decoder', short_help='Configure a slot of endpoint command decoder.')
@click.pass_obj
@click.argument('addr', type=toolbox.IntRange(0x0,0xffff))
@click.argument('slot', type=toolbox.IntRange(0x0,0x7))
@click.argument('cmd', type=toolbox.IntRange(0x0,0xff))
def configureendpointcmddecoder(obj, addr, slot, cmd):
    '''
    Configure endpoint command decoder
    '''

    lMaster = obj.mMaster
    lMaster.configure_endpoint_command_decoder(addr,slot,cmd)
# ------------------------------------------------------------------------------