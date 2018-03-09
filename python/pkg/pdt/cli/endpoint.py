import click

import collections

import pdt.cli.toolbox as toolbox

from click import echo, style, secho
from texttable import Texttable

# -----------------
def split_ints(ctx, param, value):

    sep = ','
    dash = '-'

    numbers = []
    for item in value.split(sep):
        nums = item.split(dash)
        if len(nums) == 1:
            # single entry
            numbers.append(int(item))
        elif len(nums) == 2:
            # range
            i, j = int(nums[0]), int(nums[1])
            if i > j:
                click.ClickException('Invalid interval '+item)
            numbers.extend(xrange(i,j+1))
        else:
           click.ClickException('Malformed option (comma separated list expected): {}'.format(value))

    return numbers
# -----------------

# ------------------------------------------------------------------------------
#    ____        __          _      __ 
#   / __/__  ___/ /__  ___  (_)__  / /_
#  / _// _ \/ _  / _ \/ _ \/ / _ \/ __/
# /___/_//_/\_,_/ .__/\___/_/_//_/\__/ 
#              /_/                     
@click.group('ept', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device)
@click.argument('ids', callback=split_ints)
def endpoint(obj, device, ids):
    '''
    Endpoint master commands.

    DEVICE: uhal device identifier
    IDS: enpoint ids
    '''

    lDevice = obj.mConnectionManager.getDevice(str(device))
    if obj.mTimeout:
        lDevice.setTimeoutPeriod(obj.mTimeout)

    echo('Created endpoint device ' + style(lDevice.id(), fg='blue'))

    # Ensure that target endpoint exists

    lEPNames = lDevice.getNodes('endpoint('+'|'.join( ( str(i) for i in ids ) )+')')
    if len(lEPNames) != len(ids):
        lEPNotFound = set( ( 'endpoint{}'.format(i) for i in ids ) ) -set(lEPNames)
        raise click.ClickException('Endpoints {} do(es) not exist'.format(', '.join( ('\''+ep+'\'' for ep in lEPNotFound) )))
    
    lEndpoints = { pid:lDevice.getNode('endpoint{}.version'.format(pid)) for pid in ids}

    lVersions = { pid:n.read() for pid,n in lEndpoints.iteritems()}

    lDevice.dispatch()

    # echo("Endpoint FW version: "+hex(lVersion))
    print({ k:hex(v.value()) for k,v in lVersions.iteritems()})

    lVTable = Texttable(max_width=0)
    lVTable.set_deco(Texttable.VLINES | Texttable.BORDER)
    lVTable.set_chars(['-', '|', '+', '-'])
    lVTable.header( sorted(lVersions.keys()) )
    lVTable.add_row( [hex(lVersions[p].value()) for p in sorted(lVersions.keys()) ] )
    echo  ( lVTable.draw() )

    obj.mDevice = lDevice
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@endpoint.command('enable')
@click.pass_obj
@click.option('--on/--off', default=True, help='enable/disable the endpoint')
@click.option('--partition', '-p', type=click.IntRange(0,4), help='Target partition', default=0)
def enable(obj, on, partition):
    '''
    Activate timing endpoint wrapper block.
    '''

    lEndPointNode = obj.mDevice.getNode('endpoint')
    lEndPointNode.getNode('csr.ctrl.tgrp').write(partition)
    lEndPointNode.getClient().dispatch()
    lEndPointNode.getNode('csr.ctrl.ep_en').write(on)
    lEndPointNode.getNode('csr.ctrl.buf_en').write(on)
    lEndPointNode.getClient().dispatch()
    echo("> Endpoint " + style("activated in partition {}".format(partition ) if on else "deactivated", fg='blue'))
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
# -----------------
kEpStates = collections.OrderedDict([
    (0b0000,'Standing by'), #when W_RST, -- Starting state after reset
    (0b0001,'Waiting SFP for signal'), #when W_SFP, -- Waiting for SFP LOS to go low
    (0b0010,'Waiting CDR lock'), #when W_CDR, -- Waiting for CDR lock
    (0b0011,'Waiting for comman alignment'), #when W_ALIGN, -- Waiting for comma alignment, stable 50MHz phase
    (0b0100,'Waiting for good frequency checl'), #when W_FREQ, -- Waiting for good frequency check
    (0b0101,'Waiting for 8b10 decoder good packet'), #when W_LOCK, -- Waiting for 8b10 decoder good packet
    (0b0110,'Waiting for time stamp initialisation'), #when W_RDY, -- Waiting for time stamp initialisation
    (0b1000,'Ready'), #when RUN, -- Good to go
    (0b1100,'Error in Rx'), #when ERR_R, -- Error in rx
    (0b1101,'Error in time stamp check'), #when ERR_T; -- Error in time stamp check
    ])
# -----------------

@endpoint.command('monitor', short_help='Display the status of timing endpoint.')
@click.pass_obj
@click.option('--watch', '-w', is_flag=True, default=False, help='Turn on automatic refresh')
@click.option('--period', '-p', type=click.IntRange(0, 240), default=2, help='Period of automatic refresh')
def monitor(obj, watch, period):

# def monitor(obj, watch, period):
    '''
    Display the endpoint status, accepted and rejected command counters
    '''

    lDevice = obj.mDevice
    lEndPointNode = lDevice.getNode('endpoint0')
    lTStampNode = lEndPointNode.getNode('tstamp')
    lEvCtrNode = lEndPointNode.getNode('evtctr')
    lBufCountNode = lEndPointNode.getNode('buf.count')

    while(True):
        if watch:
            click.clear()
        
        lTimeStamp = lTStampNode.readBlock(2)
        lEventCtr = lEvCtrNode.read()
        lBufCount = lBufCountNode.read()
        lDevice.dispatch()

        lTime = int(lTimeStamp[0]) + (int(lTimeStamp[1]) << 32)

        echo()
        echo( "-- " + style("Endpoint state", fg='green') + "---")
        echo()

        lCtrlDump = toolbox.readSubNodes(lEndPointNode.getNode('csr.ctrl'))
        lStatDump = toolbox.readSubNodes(lEndPointNode.getNode('csr.stat'))

        echo( "Status registers" )
        for n in sorted(lStatDump):
            echo( "  {} {}".format(n, hex(lStatDump[n])))
        echo()
        echo( 'Endpoint state: {} ({})'.format(kEpStates[int(lStatDump['ep_stat'])], hex(lStatDump['ep_stat'])))

        echo ()
        echo( "Partition: {}".format(lCtrlDump['tgrp']))
        echo( "Timestamp: {} ({})".format(style(str(lTime), fg='blue'), hex(lTime)) )
        echo( "EventCounter: {}".format(lEventCtr))
        lBufState = style('OK', fg='green') if lStatDump['buf_err'] == 0 else style('Error', fg='red')
        # lStatDump['buf_empty']
        echo( "Buffer status: " + lBufState)
        echo( "Buffer occupancy: {}".format(lBufCount))
        echo ()

        # secho ("Received commands counters", fg='green')
        # printCounters(lEndPointNode.getNode('ctrs'))
        
        echo()
        toolbox.printCounters( lEndPointNode, {
            'ctrs': 'Received counters',
            })
        if watch:
            time.sleep(period)
        else:
            break
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@endpoint.command('readback', short_help='Read the content of the endpoint master readout buffer.')
@click.pass_obj
@click.option('--events/--all', ' /-a', 'readall', default=False, help="Buffer readout mode.\n- events: only completed events are readout.\n- all: the content of the buffer is fully read-out.")
def readback(obj, readall):
    '''
    Read the content of the endpoint master readout buffer.
    '''
    lDevice = obj.mDevice
    
    lEndPointNode = lDevice.getNode('endpoint')
    lBufCount = lEndPointNode.getNode('buf.count').read()
    lEndPointNode.getClient().dispatch()

    echo ( "Words available in readout buffer: "+hex(lBufCount))
    
    # lEventsToRead = int(lBufCount) / kEventSize
    # echo (lEventsToRead)

    lWordsToRead = int(lBufCount) if readall else (int(lBufCount) / kEventSize)*kEventSize

    echo (lWordsToRead )
    if lWordsToRead == 0:
        echo("Nothing to read, goodbye!")

    lBufData = lEndPointNode.getNode('buf.data').readBlock(lWordsToRead)
    lEndPointNode.getClient().dispatch()

    for i, lWord in enumerate(lBufData):
        echo ( '{:04d} {}'.format(i, hex(lWord)))
# ------------------------------------------------------------------------------
