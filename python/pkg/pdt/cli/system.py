from __future__ import print_function

import click
import pdt.overseer

@click.group('sys', invoke_without_command=True)
@click.pass_obj
def system(obj):
    obj.overlord = pdt.overseer.Overlay.factory(obj.mConnectionManager.getDevice('SECONDARY_TUN'))
    print(dir(obj.overlord))
    obj.fanout0 = pdt.overseer.Overlay.factory(obj.mConnectionManager.getDevice('TERTIARY_TUN'))
    print(dir(obj.fanout0))


@system.command('reset')
@click.pass_obj
def reset(obj):
    obj.overlord.reset(False, None, forcepllcfg=None)


