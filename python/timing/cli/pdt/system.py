from __future__ import print_function

import click
import time
import pprint
import timing.shells
import timing.cli.pdt.toolbox as toolbox
import timing.common.database as database


from click import echo, style, secho


# ------------------------------------------------------------------------------
@click.group('ovld', invoke_without_command=True)
@click.pass_obj
def overlord(obj):
    obj.overlord = timing.shells.ShellFactory.make(obj.mConnectionManager.getDevice('OVLD_TUN'))
    obj.fanouts = {
            0:timing.shells.ShellFactory.make(obj.mConnectionManager.getDevice('FO0_TUN'))
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

    obj.overlord = timing.shells.ShellFactory.make(obj.mConnectionManager.getDevice('SECONDARY_TUN'))
    obj.fanouts = {
            0:timing.shells.ShellFactory.make(obj.mConnectionManager.getDevice('TERTIARY_FO_TUN'))
        }

    echo('--- Overlord '+obj.overlord.device.id() + ' ---')
    obj.overlord.identify()
    for id in sorted(obj.fanouts):
        echo('--- Fanout '+str(id)+' '+obj.fanouts[id].device.id() + ' ---')
        obj.fanouts[id].identify()
# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
@click.command('setup')
@click.option('--soft', '-s', is_flag=True, default=False, help='Soft reset i.e. skip the clock chip configuration.')
@click.pass_obj
def setup(obj, soft):
    obj.overlord.reset(soft=soft, forcepllcfg=None)
    for i,f in obj.fanouts.items():
        # print(i,vars(f))
        echo("Resetting fanout {}: {}".format(i,f.device.id()))
        obj.fanouts[0].reset(soft, 0, forcepllcfg=None)
    obj.overlord.synctime()
    obj.overlord.initPartitions()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@click.command('synctime')
@click.pass_obj
def synctime(obj):
    click.echo(obj.overlord.device.id())
    obj.overlord.synctime()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@click.command('scani2c')
@click.pass_obj
def scani2c(obj):
    boards = [obj.overlord] + [ obj.fanouts[k] for k in sorted(obj.fanouts)]
    for b in boards: # + obj.fanouts.values()
        echo("----"+style(b.device.id(), fg='blue')+"----")
        echo("I2C bus scan")
        for n,adrss in b.scanI2C().items():
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
    lAllBoards.update({'fanout'+str(i):f for i,f in obj.fanouts.items()})

    print(lAllBoards)
    echo('collecting data for '+','.join(lAllBoards))

    status = { n:b.status() for n,b in lAllBoards.items()}
    # freq = { n:b.freq() for n,b in lAllBoards.items()}
    pllstatus = { n:b.pllstatus() for n,b in lAllBoards.items()}

    echo( "--- " + style("Overlord IO status", fg='cyan') + " ---")
    toolbox.printRegTable(status['overlord'], False)

    echo( "--- " + style("Fanout[0] IO status", fg='cyan') + " ---")
    toolbox.printRegTable(status['fanout0'], False)

    toolbox.collateTables(
        pllstatus['overlord'][0],
        pllstatus['fanout0'][0],
        )
    toolbox.collateTables(
        toolbox.format_reg_table(pllstatus['overlord'][1], aHeader=False),
        toolbox.format_reg_table(pllstatus['fanout0'][1] , aHeader=False),
        )
    toolbox.collateTables(
        toolbox.format_reg_table(pllstatus['overlord'][2], aHeader=False),
        toolbox.format_reg_table(pllstatus['fanout0'][2] , aHeader=False),
        )
# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
@click.command('scan-fanout')
@click.pass_obj
def scanfanout(obj):

    for i,f in obj.fanouts.items():
        f.scanports()
    
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@click.command('measure-delay', short_help="Control the trigger return endpoint")
@click.option('-i','--uid', 'uids', callback=toolbox.split, help='Endpoint unique id')
@click.option('-a','--addr', 'addrs', callback=toolbox.split_ints, help='Endpoint timing address')
@click.option('-g','--grp', type=click.Choice(['wib', 'ssp', 'vst']), help='Endpoint group')
@click.option('-v', 'verbose', is_flag=True)
@click.pass_obj
def measuredelay(obj, uids, addrs, grp, verbose):


    def measure(uid, grp, taddr, fanout, slot, verbose):
        if verbose:
            echo()
            secho("Measuring alignment of {} ({}) at {} connected on fanout{} at {}".format(uid, grp, hex(taddr), fanout, slot ), fg='blue')

        lFanout.selectMux(slot)

        # Switch off all TX SFPs
        lOvld.enableEndpointSFP(0x0, False)
        time.sleep(0.1)
        # Turn on the current target
        lOvld.enableEndpointSFP(taddr)
        if verbose: echo('- Endpoint {} commanded to switch on tx'.format(hex(taddr)))
        time.sleep(0.1)

        try:
            if verbose: echo("- Locking fanout {}".format(fanout))
            lFOEptStats = lFanout.enableEptAndWaitForReady()
            if verbose: secho("fanout {} ept stats: fine delay={}, edge={}".format(fanout, *lFOEptStats), fg='cyan')
            if verbose: echo("- Locking overlord")
            lOvldEptStats = lOvld.enableEptAndWaitForReady()
            if verbose: secho("overlord ept stats: fine delay={}, edge={}".format(*lOvldEptStats), fg='cyan')

            if verbose: echo("- Measuring RTT")
            lTimeTx, lTimeRx = lOvld.send_echo_and_measure_delay()
            if verbose: secho("Measured RTT: {} clk cycles (50 MHz)".format(lTimeRx-lTimeTx), fg='green')
            if verbose: echo(" transmission {}, reception {})".format(hex(lTimeTx), hex(lTimeRx)))
            lMeasurements[uid] = (lTimeRx-lTimeTx,) + lOvldEptStats + lFOEptStats
        except:
            if verbose: secho('Failed to measure {} RTT'.format(uid))
        finally:
            lOvld.enableEndpointSFP(taddr, 0x0)


    lEndpoints = []
    for uid in uids:
        lEptInfo = database.findByUId(uid)
        if lEptInfo is None:
            secho('ERROR: Unknown endpoint uid {}'.format(uid), fg='red')
        lEndpoints.append(lEptInfo)

    for addr in addrs:
        lEptInfo = database.findByTAddr(addr)
        if lEptInfo is None:
            secho('ERROR: Unknown endpoint address {}'.format(addr), fg='red')
        lEndpoints.append(lEptInfo)
    
    if grp:
        lEndpoints += database.findByGrp(grp)

    lOvld = obj.overlord
    lFanout = obj.fanouts[0]

    lMeasurements = {}
    secho("Measuring Round Trip Time on {} endpoints".format(len(lEndpoints)))

    lEndpoints = sorted(set(lEndpoints), key=lambda x: x[0])
    if verbose:
        for lEptInfo in lEndpoints:
            measure(*lEptInfo, verbose=verbose)
    else:
        with click.progressbar(lEndpoints, show_pos=True, item_show_func=lambda x: x[0] if x is not None else '') as bar:
            for lEptInfo in bar:
                measure(*lEptInfo, verbose=verbose)

    echo()
    pprint.pprint(lMeasurements)
# ------------------------------------------------------------------------------


for cmd in [setup, synctime, status, scanfanout, measuredelay, scani2c]:
    overlord.add_command(cmd)
    vst.add_command(cmd)


