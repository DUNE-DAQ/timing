from __future__ import print_function

import click
import pdt.shells
import pdt.cli.toolbox as toolbox
import pdt.common.database as database
import time

from click import echo, style, secho


# ------------------------------------------------------------------------------
@click.group('ovld', invoke_without_command=True)
@click.pass_obj
def overlord(obj):
    obj.overlord = pdt.shells.ShellFactory.make(obj.mConnectionManager.getDevice('OVLD_TUN'))
    obj.fanouts = {
            0:pdt.shells.ShellFactory.make(obj.mConnectionManager.getDevice('FO0_TUN'))
        }
    echo('--- Overlord '+obj.overlord.device.id() + ' ---')
    obj.overlord.identify()
    for id in sorted(obj.fanouts):
        echo('--- Fanout '+str(id)+' '+obj.fanouts[id].device.id() + ' ---')
        obj.fanouts[id].identify()
    # ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@click.group('vst', invoke_without_command=True)
@click.pass_obj
def vst(obj):

    obj.overlord = pdt.shells.ShellFactory.make(obj.mConnectionManager.getDevice('SECONDARY_TUN'))
    obj.fanouts = {
            0:pdt.shells.ShellFactory.make(obj.mConnectionManager.getDevice('TERTIARY_FO_TUN'))
        }

    echo('--- Overlord '+obj.overlord.device.id() + ' ---')
    obj.overlord.identify()
    for id in sorted(obj.fanouts):
        echo('--- Fanout '+str(id)+' '+obj.fanouts[id].device.id() + ' ---')
        obj.fanouts[id].identify()
# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
@click.command('setup')
@click.pass_obj
def setup(obj):
    obj.overlord.reset(soft=False, forcepllcfg=None)
    for i,f in obj.fanouts.iteritems():
        # print(i,vars(f))
        echo("Resetting fanout {}: {}".format(i,f.device.id()))
        obj.fanouts[0].reset(False, 0, forcepllcfg=None)
    obj.overlord.synctime()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@click.command('synctime')
@click.pass_obj
def synctime(obj):
    click.echo(obj.overlord.device.id())
    obj.overlord.synctime()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@click.command('testi2c')
@click.pass_obj
def testi2c(obj):
    boards = [obj.overlord] + [ obj.fanouts[k] for k in sorted(obj.fanouts)]
    for b in boards: # + obj.fanouts.values()
        echo("----"+style(b.device.id(), fg='blue')+"----")
        echo("I2C bus scan")
        for n,adrss in b.scanI2C().iteritems():
            echo("  {}: {}".format(
                style(n, fg='cyan'),
                ', '.join([hex(a) for a in adrss])
            ))

        echo("I2C slaves ping scan")
        lPings = b.pingI2CSlaves()
        for n in sorted(lPings):
            s, a, ok = lPings[n]
            echo("  {}.{}: {} - {}".format(style(n, fg='cyan'), s, hex(a), style('OK', fg='green') if ok else style('Not responding', fg='red')))
        echo()

# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@click.command('status')
@click.pass_obj
def status(obj):

    lAllBoards = {}
    lAllBoards.update({'overlord':obj.overlord})
    lAllBoards.update({'fanout'+str(i):f for i,f in obj.fanouts.iteritems()})

    print(lAllBoards)
    echo('collecting data for '+','.join(lAllBoards))

    status = { n:b.status() for n,b in lAllBoards.iteritems()}
    # freq = { n:b.freq() for n,b in lAllBoards.iteritems()}
    pllstatus = { n:b.pllstatus() for n,b in lAllBoards.iteritems()}

    echo( "--- " + style("Overlord IO status", fg='cyan') + " ---")
    toolbox.printRegTable(status['overlord'], False)

    echo( "--- " + style("Fanout[0] IO status", fg='cyan') + " ---")
    toolbox.printRegTable(status['fanout0'], False)

    toolbox.collateTables(
        pllstatus['overlord'][0],
        pllstatus['fanout0'][0],
        )
    toolbox.collateTables(
        toolbox.formatRegTable(pllstatus['overlord'][1], aHeader=False),
        toolbox.formatRegTable(pllstatus['fanout0'][1] , aHeader=False),
        )
    toolbox.collateTables(
        toolbox.formatRegTable(pllstatus['overlord'][2], aHeader=False),
        toolbox.formatRegTable(pllstatus['fanout0'][2] , aHeader=False),
        )
# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
@click.command('scan-fanout')
@click.pass_obj
def scanfanout(obj):

    for i,f in obj.fanouts.iteritems():
        f.scanports()
    
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@click.command('measure-delay', short_help="Control the trigger return endpoint")
@click.argument('addr', type=toolbox.IntRange(0x1,0x100))
@click.option('--slot', '-s', type=click.IntRange(0,7), help='Mux select (fanout only)')
@click.pass_obj
def measuredelay(obj, addr, slot):

    lOvld = obj.overlord
    lFanout0 = obj.fanouts[0]

    fanout=0

    if slot is None:
        fanout,slot = database.kAddressToSlot[addr]
        echo("Address {} mapped to fanout {}, slot {}".format(hex(addr), fanout, slot))        

    lFanout0.selectMux(slot)

    # Switch off all TX SFPs
    lOvld.enableEndpointSFP(0x0, False)
    time.sleep(0.1)
    # Turn on the current target
    lOvld.enableEndpointSFP( addr)
    echo('Endpoint {} commanded to enable'.format(hex(addr)))
    time.sleep(0.1)


    echo("Locking fanout {}".format(fanout))
    lFanout0.enableEptAndWaitForReady()
    echo("Locking overlord")
    lOvld.enableEptAndWaitForReady()

    echo("Measuring RTT")
    lTimeTx, lTimeRx = lOvld.sendEchoAndMeasureDelay()
    echo('Measured RTT delay {} (transmission {}, reception {})'.format(lTimeRx-lTimeTx, hex(lTimeTx), hex(lTimeRx)))

    lOvld.enableEndpointSFP(addr, 0x0)

# ------------------------------------------------------------------------------


for cmd in [setup, synctime, status, scanfanout, measuredelay, testi2c]:
    overlord.add_command(cmd)
    vst.add_command(cmd)


