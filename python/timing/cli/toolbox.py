from __future__ import print_function

import re

import timing.common.definitions as defs
import click
import time

from click import echo, style, secho

# ------------------------------------------------------------------------------
def hookDebugger(debugger='gdb'):
    '''debugging helper, hooks debugger to running interpreter process'''

    import os
    pid = os.spawnvp(os.P_NOWAIT,
                     debugger, [debugger, '-q', 'python', str(os.getpid())])

    # give debugger some time to attach to the python process
    import time
    time.sleep( 1 )

    # verify the process' existence (will raise OSError if failed)
    os.waitpid( pid, os.WNOHANG )
    os.kill( pid, 0 )
    return
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
class IntRange(click.types.IntParamType):
    """A parameter that works similar to :data:`click.INT` but restricts
    the value to fit into a range.  The default behavior is to fail if the
    value falls outside the range, but it can also be silently clamped
    between the two edges.

    See :ref:`ranges` for an example.
    """
    name = 'integer range'

    def __init__(self, min=None, max=None, clamp=False):
        self.min = min
        self.max = max
        self.clamp = clamp

    def convert(self, value, param, ctx):
        
        if type(value) == str:
            if value.startswith('0x'):
                base = 16
            elif value.startswith('0o'):
                bae = 8
            elif value.startswith('0b'):
                base = 2   
            else:
                base = 10
            rv = int(value, base)
        else:
            rv = int(value)
        if self.clamp:
            if self.min is not None and rv < self.min:
                return self.min
            if self.max is not None and rv > self.max:
                return self.max
        if self.min is not None and rv < self.min or \
           self.max is not None and rv > self.max:
            if self.min is None:
                self.fail('%s is bigger than the maximum valid value '
                          '%s.' % (rv, self.max), param, ctx)
            elif self.max is None:
                self.fail('%s is smaller than the minimum valid value '
                          '%s.' % (rv, self.min), param, ctx)
            else:
                self.fail('%s is not in the valid range of %s to %s.'
                          % (rv, self.min, self.max), param, ctx)
        return rv

    def __repr__(self):
        return 'IntRange(%r, %r)' % (self.min, self.max)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def sanitizeConnectionPaths(aConnectionPaths):

    lConnectionList = aConnectionPaths.split(';')
    for i,c in enumerate(lConnectionList):
        if re.match('^\w+://.*', c) is None:
            lConnectionList[i] = 'file://'+c
    return ';'.join(lConnectionList)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def completeDevices(ctx, args, incomplete):
    import uhal
    root_ctx = ctx.find_root()
    devs = uhal.ConnectionManager(sanitizeConnectionPaths(str(root_ctx.params['connections']))).getDevices()
    return [k for k in devs if incomplete in k]
    # return []
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def readSubNodes(aNode, dispatch=True):
    lValues = { n:aNode.getNode(n).read() for n in aNode.getNodes() }

    if dispatch:
        aNode.getClient().dispatch()
    return lValues
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def resetSubNodes(aNode, aValue=0x0, dispatch=True):
    """
    Reset subnodes of aNode to aValue
    """
    lValues = { n:aNode.getNode(n).write(aValue) for n in aNode.getNodes() }
    if dispatch:
        aNode.getClient().dispatch()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def hookDebugger(debugger='gdb'):
    '''
    debugging helper, hooks debugger to running interpreter process
    '''

    import os
    pid = os.spawnvp(os.P_NOWAIT,
                     debugger, [debugger, '-q', 'python', str(os.getpid())])

    # give debugger some time to attach to the python process
    import time
    time.sleep( 1 )

    # verify the process' existence (will raise OSError if failed)
    os.waitpid( pid, os.WNOHANG )
    os.kill( pid, 0 )
    return
# ------------------------------------------------------------------------------

# -----------------
def validate_device(ctx, param, value):

    lDevices = ctx.obj.mConnectionManager.getDevices()
    if value not in lDevices:
        raise click.BadParameter(
            'Device must be one of '+
            ', '.join(["'"+lId+"'" for lId in lDevices])
            )
    return value
# -----------------


# -----------------
def complete_device(ctx, args, incomplete):
    lDevices = ctx.obj.mConnectionManager.getDevices()

    return [k for k in lDevices if incomplete in k]
# -----------------

# ------------------------------------------------------------------------------
def split(ctx, param, value):
    if value is None:
        return []

    return value.split(',')
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def __str2int__( value ):
    if value.startswith('0x'):
        base = 16
    elif value.startswith('0o'):
        bae = 8
    elif value.startswith('0b'):
        base = 2   
    else:
        base = 10
    return int(value, base)

def split_ints(ctx, param, value):

    sep = ','
    dash = '-'

    if value is None:
        return []

    numbers = []
    for item in value.split(sep):
        nums = item.split(dash)
        if len(nums) == 1:
            # single entry
            numbers.append(__str2int__(item))
        elif len(nums) == 2:
            # range
            i, j = __str2int__(nums[0]), __str2int__(nums[1])
            if i > j:
                click.ClickException('Invalid interval '+item)
            numbers.extend(list(range(i,j+1)))
        else:
           click.ClickException('Malformed option (comma separated list expected): {}'.format(value))

    return numbers
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def printRegTable(aRegs, aHeader=True, sort=True):
    echo  ( format_reg_table(aRegs, aHeader, sort) )
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def printDictTable(aDict, aHdr=True, aSort=True, aFmtr=None):
    echo  ( formatDictTable(aDict, aHdr, aSort, aFmtr) )
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def collateTables(t1, t2):
    l1 = t1.split('\n')
    l2 = t2.split('\n')

    col1 = max([len(escape_ansi(l)) for l in l1])
    col2 = max([len(escape_ansi(l)) for l in l2])

    nrows = max(len(l1), len(l2));

    l1 += [''] * (nrows - len(l1))
    l2 += [''] * (nrows - len(l2))
    fmt = '\'{:<%d}\' \'{:<%d}\'' % (col1, col2)
    for c1,c2 in zip(l1, l2):
        print (c1 + ' '*(col1-len(escape_ansi(c1))), '  ' ,c2 + ' '*(col2-len(escape_ansi(c2))))
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
kReEscapeAnsi = re.compile(r'(\x9B|\x1B\[)[0-?]*[ -/]*[@-~]')

def escape_ansi(line):
    return kReEscapeAnsi.sub('', line)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def formatTStamp( aRawTStamp, clock_frequency_hz ):
    ts = int(aRawTStamp[0]) + int((aRawTStamp[1]) << 32)
    
    lSubSec = ts % clock_frequency_hz
    lSecFromEpoch = ts / clock_frequency_hz

    return time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.localtime(lSecFromEpoch))
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def tstamp2int( aRawTStamp ):
    return int(aRawTStamp[0]) + int((aRawTStamp[1]) << 32)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def fmtEpState(aState):
    aState = aState.value()
    return '{} ({})'.format(defs.kEpStates[aState], hex(aState)) if aState in defs.kEpStates else hex(aState)
# ------------------------------------------------------------------------------