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
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@overlord.command('setup')
@click.pass_obj
def setup(obj):
    obj.overlord.reset(False, forcepllcfg=None)
    for i,f in obj.fanouts.iteritems():
        echo("Resetting fanout {}: {}".format(i,f.id()))
        obj.fanout0.reset(False, 0, forcepllcfg=None)
    obj.overlord.synctime()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@overlord.command('synctime')
@click.pass_obj
def synctime(obj):
    click.echo(obj.overlord.device.id())
    obj.overlord.synctime()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@overlord.command('status')
@click.pass_obj
def status(obj):

    

    pass

# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
@overlord.command('measure-delay', short_help="Control the trigger return endpoint")
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
    echo('Measured RTT delay {} (transmission {}, reception {})'.format(hex(lTimeTx), hex(lTimeRx), lTimeRx-lTimeTx))

    lOvld.enableEndpointSFP(addr, 0x0)

# ------------------------------------------------------------------------------


