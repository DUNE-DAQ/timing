from __future__ import print_function
import click
import time
from io import StringIO
import traceback

# PDT imports
import timing.cli.toolbox as toolbox
import timing.common.definitions as defs
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignOverlord

from click import echo, style, secho

from .master import master

# ------------------------------------------------------------------------------
# -- cyc_len and spill_len are in units of 1 / (50MHz / 2^24) = 0.34s
@master.group('ext-trig', short_help='Extenal trigger command group.')
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

    if obj.mDesignType == kDesignOverlord:
        lExtTrig = obj.mExtTrig
    
        if action == 'disable':
            lExtTrig.disable()
        elif action == 'enable':
            lExtTrig.enable()
        elif action == 'reset':
            lExtTrig.reset()
    
        secho("Trigger endpoint action '" + action + "' completed", fg='green')
        ctx.invoke(exttrg_status)
    else:
        secho("External triggers only available on overlord design", fg='red')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@externaltrigger.command('enable', short_help="Enable external triggers")
@click.option('--on/--off', default=True, help='enable/disable triggers')
@click.pass_obj
@click.pass_context
def exttrg_enable(ctx, obj, on):

    if obj.mDesignType == kDesignOverlord:
        lExtTrig = obj.mExtTrig

        if on:
            lExtTrig.enable_triggers()
        else:
            lExtTrig.disable_triggers()
        secho("External triggers " + ("enabled" if on else "disabled"), fg='green')
        ctx.invoke(exttrg_status)
    else:
        secho("External triggers only available on overlord design", fg='red')
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@externaltrigger.command('status', short_help="Monitor trigger input status")
@click.option('--watch', '-w', is_flag=True, default=False, help='Turn on automatic refresh')
@click.option('--period','-p', type=click.IntRange(0, 10), default=2, help='Automatic refresh period')
@click.pass_obj
def exttrg_status(obj, watch, period):
    
    if obj.mDesignType == kDesignOverlord:
        lExtTrig = obj.mExtTrig

        while(True):
            if watch:
                click.clear()
            
            echo()
    
            echo(lExtTrig.get_status())
            
            if watch:
                time.sleep(period)
            else:
                break
    else:
        secho("External triggers only available on overlord design", fg='red')
# ------------------------------------------------------------------------------
