import click
import sys
import time
import array
import collections
import h5py
from datetime import datetime

from . import toolbox
import timing.common.definitions as defs
from timing.common.definitions import kLibrarySupportedBoards, kLibrarySupportedDesigns

from click import echo, style, secho
import time

# ------------------------------------------------------------------------------
#    ____        __          _      __ 
#   / __/__  ___/ /__  ___  (_)__  / /_
#  / _// _ \/ _  / _ \/ _ \/ / _ \/ __/
# /___/_//_/\_,_/ .__/\___/_/_//_/\__/ 
#              /_/                     
@click.group('hsi', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device, shell_complete=toolbox.completeDevices)
def hsi(obj, device):
    '''
    HSI commands.

    \b
    DEVICE: uhal device identifier
    IDS: id(s) of the target endpoint(s).
    '''

    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)

    
    echo('Created HSI device')
    lTopDesign = lDevice.getNode('')
    lBoardInfo = toolbox.readSubNodes(lDevice.getNode('io.config'), False)
    lDevice.dispatch()

    if lBoardInfo['board_type'].value() in kLibrarySupportedBoards and lBoardInfo['design_type'].value() in kLibrarySupportedDesigns:
        lTopDesign.validate_firmware_version()
        try:
            echo(lDevice.getNode('io').get_hardware_info())
        except:
            secho("Failed to retrieve hardware information! I2C issue? Initial board reset needed?", fg='yellow')
            e = sys.exc_info()[0]
            secho("Error: {}".format(e), fg='red')

    obj.mDevice = lDevice
    obj.mEndpoint = lDevice.getNode('endpoint0')
    obj.mTopDesign = lDevice.getNode('')
    obj.mHSI = obj.mTopDesign.get_hsi_node()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@hsi.command('status')
@click.pass_obj
@click.pass_context
def status(ctx, obj):
    '''
    Print the status of CRT endpoint wrapper block.
    '''

    lDevice = obj.mDevice
    lHSI = obj.mHSI
    lEndpoint = obj.mEndpoint
    
    echo(lEndpoint.get_status())
    echo(lHSI.get_status())
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@hsi.command('enable', short_help="Configure the HSI endpoint for running")
@click.pass_obj
@click.pass_context
@click.argument('action', default='on', type=click.Choice(['on', 'off', 'reset']))
@click.option('--partition', '-p', type=click.IntRange(0,4), help='Partition', default=0)
@click.option('--address', '-a', type=toolbox.IntRange(0x0,0x100), help='Address', default=0)
def enable(ctx, obj, action, partition, address):
    '''
    Activate the timing endpoint in the hsi design. Left in for compatibility reasons
    '''

    lDevice = obj.mDevice
    lEndpoint = obj.mEndpoint
    lHSI = obj.mHSI
    
    if action == 'off':
        lEndpoint.disable()
    elif action == 'on':
        lEndpoint.enable(address=address,partition=partition)
    elif action == 'reset':
        lEndpoint.reset(address=address,partition=partition)
        lHSI.reset_hsi()

    time.sleep(0.1)
    ctx.invoke(status)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@hsi.command('configure', short_help="Configure the HSI block for running")
@click.pass_obj
@click.pass_context
@click.option('--src', '-s', type=click.IntRange(0x0,0x1), help='Source of HSI data,; 0: hardware; 1: timestamp (emulation mode)', default=0)
@click.option('--re-mask', '-r', type=click.IntRange(0,0xffffffff), help='Rising edge mask', default=0)
@click.option('--fe-mask', '-f', type=click.IntRange(0,0xffffffff), help='Falling edge mask', default=0)
@click.option('--inv-mask', '-i', type=click.IntRange(0,0xffffffff), help='Invert mask', default=0)
@click.option('--rate', type=float, help='Random trigger rate [Hz] on bit 0 in emulation mode', default=1)
def configure(ctx, obj, src, re_mask, fe_mask, inv_mask, rate):
    '''
    Configure the hsi in the hsi wrapper block.
    '''

    lDevice = obj.mDevice
    lHSI = obj.mHSI
    lTopDesign = obj.mTopDesign

    lHSI.reset_hsi()
    lTopDesign.configure_hsi(src, re_mask, fe_mask, inv_mask, rate)
    lHSI.start_hsi()
    secho("HSI configured (and started)", fg='green')

    time.sleep(0.1)
    ctx.invoke(status)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@hsi.command('start', short_help='Start the hsi triggering and the writing events into buffer.')
@click.pass_obj
@click.pass_context
def readback(ctx, obj):
    '''
    Read the content of the endpoint master readout buffer.
    '''
    lDevice = obj.mDevice
    lHSI = obj.mHSI

    lHSI.start_hsi()
    secho("HSI start", fg='green')

    time.sleep(0.1)
    ctx.invoke(status)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@hsi.command('stop', short_help='Stop the hsi triggering and the writing events into buffer.')
@click.pass_obj
@click.pass_context
def readback(ctx, obj):
    '''
    Read the content of the endpoint master readout buffer.
    '''
    lDevice = obj.mDevice
    lHSI = obj.mHSI

    lHSI.stop_hsi()
    secho("HSI stop", fg='green')

    time.sleep(0.1)
    ctx.invoke(status)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@hsi.command('read', short_help='Read the content of the hsi readout buffer.')
@click.pass_obj
@click.pass_context
@click.option('--all/--events', '-a/ ', 'readall', default=False, help="Buffer readout mode.\n- events: only completed events are readout.\n- all: the content of the buffer is fully read-out.")
@click.option('--continuous', '-c', 'continuous', is_flag=True, default=False, help="Read data continuously")
@click.option('--print', '-p', 'print_out', is_flag=True, default=False, help="Print event data")
@click.option('--read-period', '-r', type=int, help='Period of readout [ms]', default=100)
@click.option('--save', '-s', is_flag=True, default=False, help='Flag controlling data saving to file')
@click.option('--file-name', '-f', type=click.Path(), help='Name of file to store data in')
def read(ctx, obj, readall, continuous, print_out, read_period, save, file_name):
    '''
    Read the content of the endpoint master readout buffer.
    '''
    lDevice = obj.mDevice
    lHSI = obj.mHSI

    # TODO get from HSINode class
    n_words_per_hsi_buffer_event=5
    
    read_events=0
    if continuous:
        secho("[INFO] Starting HSI data readout", fg='green')
        if not (save or print_out):
            secho('[WARNING] you have chosen to neither save or print the HSI data', fg='yellow')
        last_ts=0
        if save:
            h5_file_name = datetime.now().strftime("hsi_data_%d_%m_%Y_%H_%M_%S.hdf5")
            if file_name is not None:
                h5_file_name=file_name

            f = h5py.File(h5_file_name, "w")
            hsi_dataset = f.create_dataset("hsi_frames", (0,7), maxshape=(None, 7), chunks=True, dtype='u4')
        else:
            if file_name is not None:
                secho('[WARNING] You have provided a file name but chosen not to save the readout data', fg='yellow')

        with toolbox.InterruptHandler() as h:
            while(True):
                n_words=0
                hsi_words = lHSI.read_data_buffer(n_words,False,False)
                n_hsi_events = len(hsi_words) // n_words_per_hsi_buffer_event

                if save:
                    hsi_dataset_start_index=hsi_dataset.shape[0]
                    hsi_dataset.resize(hsi_dataset_start_index+n_hsi_events, axis=0)

                if (len(hsi_words) % n_words_per_hsi_buffer_event == 0 and hsi_words.size() > 0):

                    for i in range(0,n_hsi_events):
                        start_index = i * n_words_per_hsi_buffer_event
                        end_index = start_index + n_words_per_hsi_buffer_event
                        raw_event=hsi_words[start_index:end_index]

                        header = raw_event[0]
                        ts_low = raw_event[1]
                        ts_high = raw_event[2]
                        data = raw_event[3]
                        trigger = raw_event[4]

                        ts = ts_low | (ts_high << 32)

                        # bits 15-0 contain the sequence counter
                        counter = header & 0x0000ffff

                        if (header >> 16) != 0xaa00:
                            secho("[ERROR] Invalid HSIEventHeader", fg='red')
                            continue

                        if ts == 0:
                            secho("[ERROR] Invalid HSIEvent timestamp", fg='red')
                            continue

                        delta_ticks=ts-last_ts
                        if last_ts == 0:
                            delta_ticks=0
                        last_ts=ts

                        if print_out:
                            print(f"[INFO] got event: {counter} ts: {ts} signals: {data} delta ts: {delta_ticks}")

                        read_events=read_events+1
                        if save:
                            hsi_dataset[hsi_dataset_start_index+i,0]=(0x1 << 6) | 0x1 # DAQHeader, frame version: 1, det id: 1
                            hsi_dataset[hsi_dataset_start_index+i,1]=ts_low
                            hsi_dataset[hsi_dataset_start_index+i,2]=ts_high
                            hsi_dataset[hsi_dataset_start_index+i,3]=data
                            hsi_dataset[hsi_dataset_start_index+i,4]=0x0
                            hsi_dataset[hsi_dataset_start_index+i,5]=trigger
                            hsi_dataset[hsi_dataset_start_index+i,6]=counter

                elif len(hsi_words) != 0:
                    secho(f"[ERROR] unexpected n words {len(hsi_words)}", fg='red')
                if h.interrupted:
                    secho(f"[INFO] Stopping HSI data readout after {read_events} events", fg='green')
                    break
                time.sleep(read_period*1e-3)
        if save:
            f.close()
    else:
        echo(lHSI.get_data_buffer_table(readall,False))
# ------------------------------------------------------------------------------