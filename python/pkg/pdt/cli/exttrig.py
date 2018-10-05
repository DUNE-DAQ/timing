from __future__ import print_function
import click
import time
import StringIO
import traceback

# PDT imports
import pdt.cli.toolbox as toolbox
import pdt.cli.definitions as defs

from click import echo, style, secho

from master import master

# ------------------------------------------------------------------------------
# -- cyc_len and spill_len are in units of 1 / (50MHz / 2^24) = 0.34s
@master.group('ext-trig', short_help='Extenal trigger command group')
@click.pass_obj
def externaltrigger(obj):
    pass
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@externaltrigger.command('ept', short_help="Control the trigger return endpoint")
@click.argument('action', default='enable', type=click.Choice(['enable', 'disable', 'reset']))
@click.pass_obj
@click.pass_context
def exttrg_ept(ctx, obj, action):

    lExtTrig = obj.mExtTrig

    # Disable comes first
    if action in ['disable','reset']:
        lExtTrig.getNode('csr.ctrl.ep_en').write(0x0)
    # And then enable
    if action in ['enable','reset']:
        lExtTrig.getNode('csr.ctrl.ep_en').write(0x1)

    lExtTrig.getClient().dispatch()
    secho("Trigger endpoint action '" + action + "' completed", fg='green')

    ctx.invoke(exttrg_status)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@externaltrigger.command('enable', short_help="Enable external triggers")
@click.option('--on/--off', default=True, help='enable/disable triggers')
@click.pass_obj
@click.pass_context
def exttrg_enable(ctx, obj, on):

    lExtTrig = obj.mExtTrig

    lExtTrig.getNode('csr.ctrl.ext_trig_en').write(on)
    lExtTrig.getClient().dispatch()
    secho("External triggers " + ("enabled" if on else "disabled"), fg='green')
    ctx.invoke(exttrg_status)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@externaltrigger.command('status', short_help="Monitor trigger input status")
@click.option('--watch', '-w', is_flag=True, default=False, help='Turn on automatic refresh')
@click.option('--period','-p', type=click.IntRange(0, 10), default=2, help='Automatic refresh period')
@click.pass_obj
def exttrg_status(obj, watch, period):
    
    lExtTrig = obj.mExtTrig

    while(True):
        if watch:
            click.clear()
        
        echo()

        secho('External trigger endpoint', fg='blue')
        echo()
        
        lCtrlDump = toolbox.readSubNodes(lExtTrig.getNode('csr.ctrl'))
        lStatDump = toolbox.readSubNodes(lExtTrig.getNode('csr.stat'))

        echo( "Endpoint State: {}".format(style(defs.fmtEpState(lStatDump['ep_stat']), fg='blue')))
        echo()
        secho("Control registers", fg='cyan')
        toolbox.printRegTable(lCtrlDump, False)
        echo()
        secho("Status registers", fg='cyan')
        toolbox.printRegTable(lStatDump, False)
        echo()

        toolbox.printCounters( lExtTrig, {
            'ctrs': 'Counters',
        })
        if watch:
            time.sleep(period)
        else:
            break
# ------------------------------------------------------------------------------

