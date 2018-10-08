from __future__ import print_function

import click
import pdt.shells
import pdt.cli.toolbox as toolbox
import time

from click import echo, style, secho

kSSPAddresses = {
    (0,5): range(0x20, 0x24),
    (0,6): range(0x24, 0x28),
    (0,1): range(0x28, 0x2c),
    (0,2): range(0x2c, 0x30),
    (0,4): range(0x30, 0x34),
    (0,3): range(0x34, 0x38),
}

kWibAddresses = {
    (0,5): range(0x45, 0x4a),
    (0,6): range(0x4a, 0x4f),
    (0,1): range(0x4f, 0x54),
    (0,2): range(0x54, 0x59),
    (0,4): range(0x59, 0x5e),
    (0,3): range(0x5e, 0x63),
}





# ------------------------------------------------------------------------------
@click.group('ovld', invoke_without_command=True)
@click.pass_obj
def overlord(obj):
    obj.overlord = pdt.shells.ShellFactory.make(obj.mConnectionManager.getDevice('SECONDARY_TUN'))
    obj.fanouts = {
            0:pdt.shells.ShellFactory.make(obj.mConnectionManager.getDevice('TERTIARY_FO_TUN'))
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

    # 

    pass

# ------------------------------------------------------------------------------



# ------------------------------------------------------------------------------
@overlord.command('measure-delay', short_help="Control the trigger return endpoint")
@click.argument('addr', type=toolbox.IntRange(0x0,0x100))
@click.option('--mux', '-m', type=click.IntRange(0,7), help='Mux select (fanout only)')
@click.pass_obj
def measuredelay(obj, addr, mux):

    lOvld = obj.overlord
    lFanout0 = obj.fanout0
    if mux is not None:
        lFanout0.selectMux(mux)

    # Switch off all TX SFPs
    lOvld.enableEndpointSFP(0x0, False)
    time.sleep(0.1)
    # Turn on the current target
    lOvld.enableEndpointSFP( addr)
    echo('Endpoint {} commanded to enable'.format(hex(addr)))
    time.sleep(0.1)


    echo('Locking fanout 0')
    lFanout0.enableEptAndWaitForReady()
    echo('Locking overlord')
    lOvld.enableEptAndWaitForReady()

    echo('Measuring RTT')
    lTimeTx, lTimeRx = lOvld.sendEchoAndMeasureDelay()
    print(lTimeTx, lTimeRx, lTimeRx-lTimeTx)

    lOvld.enableEndpointSFP(addr, 0x0)

# ------------------------------------------------------------------------------


