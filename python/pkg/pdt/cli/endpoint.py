import click

import collections

import toolbox
import definitions as defs

from click import echo, style, secho
from click_texttable import Texttable
import time


# ------------------------------------------------------------------------------
#    ____        __          _      __ 
#   / __/__  ___/ /__  ___  (_)__  / /_
#  / _// _ \/ _  / _ \/ _ \/ / _ \/ __/
# /___/_//_/\_,_/ .__/\___/_/_//_/\__/ 
#              /_/                     
@click.group('ept', invoke_without_command=True)
@click.pass_obj
@click.argument('device', callback=toolbox.validate_device)
@click.argument('ids', callback=toolbox.split_ints)
def endpoint(obj, device, ids):
    '''
    Endpoint master commands.

    \b
    DEVICE: uhal device identifier
    IDS: id(s) of the target endpoint(s).
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
    
    lEndpoints = { pid:lDevice.getNode('endpoint{}'.format(pid)) for pid in ids}

    lVersions = { pid:n.getNode('version').read() for pid,n in lEndpoints.iteritems()}

    lDevice.dispatch()

    if len(set( (v.value() for v in lVersions.itervalues()) )) > 1:
        secho('WARNING: multiple enpoint versions detected', fg='yellow')
        secho()

        lVTable = Texttable(max_width=0)
        lVTable.set_deco(Texttable.VLINES | Texttable.BORDER)
        lVTable.set_chars(['-', '|', '+', '-'])
        lVTable.header( sorted(lVersions.keys()) )
        lVTable.add_row( [hex(lVersions[p].value()) for p in sorted(lVersions.keys()) ] )
        echo  ( lVTable.draw() )

    obj.mDevice = lDevice
    obj.mEndpoints = lEndpoints
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@endpoint.command('enable')
@click.pass_obj
@click.option('--on/--off', default=True, help='enable/disable the endpoint')
@click.option('--partition', '-p', type=click.IntRange(0,4), help='Partition', default=0)
@click.option('--address', '-a', type=toolbox.IntRange(0x0,0x100), help='Address', default=None)
def enable(obj, on, partition, address):
    '''
    Activate timing endpoint wrapper block.
    '''

    lDone = []
    for i, ep in obj.mEndpoints.iteritems():
        if on:
            ep.getNode('csr.ctrl.tgrp').write(partition)
            if address is not None:
                ep.getNode('csr.ctrl.int_addr').write(0x1)
                ep.getNode('csr.ctrl.addr').write(address)
            else:
                ep.getNode('csr.ctrl.int_addr').write(0x0)

            ep.getNode('csr.ctrl.ctr_rst').write(0x1)
            ep.getNode('csr.ctrl.ctr_rst').write(0x0)
            ep.getClient().dispatch()
        ep.getNode('csr.ctrl.ep_en').write(on)
        ep.getNode('csr.ctrl.buf_en').write(on)
        ep.getClient().dispatch()
        lDone.append(i)

    echo(
        "> Endpoints {} ".format(','.join( (str(i) for i in lDone) )) + style("activated in partition {}".format(partition) if on else "deactivated", fg='blue'))
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------



@endpoint.command('status', short_help='Display the status of timing endpoint.')
@click.pass_obj
@click.option('--watch', '-w', is_flag=True, default=False, help='Turn on automatic refresh')
@click.option('--period', '-p', type=click.IntRange(0, 240), default=2, help='Period of automatic refresh')
def status(obj, watch, period):
    '''
    Display the endpoint status, accepted and rejected command counters
    '''
    lNumCtrs = 0x10

    lDevice = obj.mDevice
    lEndpoints = obj.mEndpoints
    lEndPointNode = lDevice.getNode('endpoint0')



    lTStampNode = lEndPointNode.getNode('tstamp')
    lEvCtrNode = lEndPointNode.getNode('evtctr')
    lBufCountNode = lEndPointNode.getNode('buf.count')


    while(True):
        if watch:
            click.clear()
        
        lEPKeys = sorted(lEndpoints)
        lEPData = { p:{} for p,_ in lEndpoints.iteritems() }

        for p,n in lEndpoints.iteritems():

            lData = lEPData[p]
            lData['tstamp'] = n.getNode('tstamp').readBlock(2)
            lData['evtctr'] = n.getNode('evtctr').read()
            lData['bufcount'] = n.getNode('buf.count').read()

            lData['ctrldump'] = toolbox.readSubNodes(n.getNode('csr.ctrl'), False)
            lData['statdump'] = toolbox.readSubNodes(n.getNode('csr.stat'), False)
            lData['ctrs'] = n.getNode('ctrs').readBlock(lNumCtrs)
        lDevice.dispatch()


        lTimeStamp = lTStampNode.readBlock(2)
        lDevice.dispatch()


        lEPSummary = Texttable(max_width=0)
        lEPSummary.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER )
        lEPSummary.set_chars(['-', '|', '+', '-'])
        lEPSummary.header( ['Endpoint']+lEPKeys )
        lEPSummary.set_cols_dtype(['t']*(len(lEPKeys)+1))
        lEPSummary.add_row(
                ['State']+
                [defs.fmtEpState(lEPData[p]['statdump']['ep_stat']) for p in lEPKeys
                ]
        )
        lEPSummary.add_row(
                ['Partition']+
                [str(lEPData[p]['ctrldump']['tgrp']) for p in lEPKeys]
        )
        lEPSummary.add_row(
                ['Address']+
                [str(lEPData[p]['ctrldump']['addr']) for p in lEPKeys]
        )
        lEPSummary.add_row(
                ['Timestamp']+
                [style(str(toolbox.formatTStamp(lEPData[p]['tstamp'])), fg='blue') for p in lEPKeys]
        )
        lEPSummary.add_row(
                ['Timestamp (hex)']+
                [hex(toolbox.formatTStamp(lEPData[p]['tstamp'])) for p in lEPKeys]
        )        
        lEPSummary.add_row(
                ['EventCounter']+
                [str(lEPData[p]['evtctr']) for p in lEPKeys]
        )
        lEPSummary.add_row(
                ['Buffer status']+
                [style('OK', fg='green') if (lEPData[p]['statdump']['buf_err'] == 0x0) else style('Error', fg='red') for p in lEPKeys]
        )
        lEPSummary.add_row(
                ['Buffer occupancy']+
                [str(lEPData[p]['bufcount']) for p in lEPKeys]
        )
        echo ( lEPSummary.draw() )

        echo()
        echo( "--- " + style("Endpoint state", fg='cyan') + " ---")

        lEPStats = Texttable(max_width=0)
        lEPStats.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
        lEPStats.set_chars(['-', '|', '+', '-'])
        lEPStats.set_cols_align(['l']+['c']*len(lEPKeys))
        lEPStats.set_cols_width([10]+[8]*(len(lEPKeys)))

        lEPStats.header( ['Endpoint']+lEPKeys )

        for k in sorted(lEPData[lEPKeys[0]]['statdump']):
            lEPStats.add_row(
                [k]+
                [
                    style(hex(v), fg=('blue' if v != 0 else 'white')) 
                    for v in ( lEPData[p]['statdump'][k]  for p in lEPKeys ) 
                ]
            )
        echo ( lEPStats.draw() )

        echo()
        echo( "--- " + style("Command counters", fg='cyan') + " ---")

        lEPCtrs = Texttable(max_width=0)
        lEPCtrs.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
        lEPCtrs.set_chars(['-', '|', '+', '-'])
        lEPCtrs.set_cols_align(['l']+['c']*len(lEPKeys))
        lEPCtrs.set_cols_width([10]+[8]*(len(lEPKeys)))

        lEPCtrs.header( ['Endpoint']+lEPKeys )
        for c in xrange(lNumCtrs):
            lEPCtrs.add_row(
                [defs.kCommandNames.get(c)]+
                [k if k > 0 else '' for k in (lEPData[p]['ctrs'][c] for p in lEPKeys)]
                )
        echo ( lEPCtrs.draw() )

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
