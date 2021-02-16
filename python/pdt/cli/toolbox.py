from __future__ import print_function

import re

from . import definitions as defs
import click
import time

from click import echo, style, secho
from .click_texttable import Texttable


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


# -----------------
def validate_partition(ctx, param, value):
    
    lMaxParts = ctx.obj.mGenerics['n_part']
    if value < 0 or value > lMaxParts-1:

        raise click.BadParameter(
            'Invalid partition {}. Available partitions: {}'.format(value, list(range(lMaxParts)))
        )
    return value
# -----------------


# ------------------------------------------------------------------------------
def validate_chan(ctx, param, value):
    
    lMaxChans = ctx.obj.mGenerics['n_chan']
    if value < 0 or value > lMaxChans-1:

        raise click.BadParameter(
            'Invalid partition {}. Available partitions: {}'.format(value, list(range(lMaxChans)))
        )
    return value
# ------------------------------------------------------------------------------

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
def format_reg_table(aRegs, aHeader=True, sort=True):
    lRegTable = Texttable(max_width=0)
    lRegTable.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
    lRegTable.set_chars(['-', '|', '+', '-'])
    if aHeader:
        lRegTable.header( ['name', 'value'] )

    lRegs = sorted(aRegs) if sort else aRegs
    for k in lRegs:
        lRegTable.add_row( [str(k), hex(aRegs[k])] )
        
    return lRegTable.draw()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def printDictTable(aDict, aHdr=True, aSort=True, aFmtr=None):
    echo  ( formatDictTable(aDict, aHdr, aSort, aFmtr) )
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def formatDictTable(aDict, aHdr=True, aSort=True, aFmtr=str):
    lDictTable = Texttable(max_width=0)
    lDictTable.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
    lDictTable.set_chars(['-', '|', '+', '-'])
    if aHdr:
        lDictTable.header( ['name', 'value'] )

    for k in (sorted(aDict) if aSort else aDict):
        v = aDict[k]
        lDictTable.add_row( [str(k), aFmtr(v) if aFmtr else v])
        
    return lDictTable.draw()
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
def formatTStamp( aRawTStamp ):
    ts = int(aRawTStamp[0]) + int((aRawTStamp[1]) << 32)
    
    lSubSec = ts % defs.kSPSClockInHz
    lSecFromEpoch = ts / defs.kSPSClockInHz

    return time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.localtime(lSecFromEpoch))
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def tstamp2int( aRawTStamp ):
    return int(aRawTStamp[0]) + int((aRawTStamp[1]) << 32)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def printCounters( aTopNode, aSubNodes, aNumCtrs=0x10, aTitle='Cmd', aLegend=defs.kCommandNames ):

    lBlocks = []

    # try:
    #     aTopNode.getClient().dispatch()
    # except uhal.exception as e:
    #     for b in lBlocks:
    #         print (b.valid())
    #     raise SystemExit(0)
    
    for lKey in aSubNodes:
        try:
            ctrs = aTopNode.getNode(lKey).readBlock(aNumCtrs)
            aTopNode.getClient().dispatch()
            lBlocks.append(ctrs.value())

        except uhal.exception as e:
            print ('Failed to read ', lKey)
            lBlocks.append([None]*aNumCtrs)

    # Just a bit of math
    lCellWidth = 12
    kCellFmt = ' {{:^{}}} '.format(lCellWidth)
    kTitleCellFmt = ' {{:^{}}} '.format((lCellWidth+1)*2+1)

    lLineLen = (lCellWidth+2+1)*(len(aSubNodes)*2+1)+1

    # Build the title
    lLine = [kCellFmt.format('')]+[style(kTitleCellFmt.format(aSubNodes[lName]), fg='green') for lName in aSubNodes]
 
    lTitle = '|'.join(['']+lLine+[''])
    echo ( '-'*lLineLen)
    # print ( '-'*len(lTitle))
    echo ( lTitle )

    # Build the title
    lLine = [aTitle] +( ['cnts', 'hex' ]*len(aSubNodes) )
    lHdr = '|'.join(['']+[kCellFmt.format(lCell) for lCell in lLine]+[''])
    print ( '-'*lLineLen)
    print ( lHdr )
    print ( '-'*lLineLen)

    for lId in range(aNumCtrs):

        lLine = [ (aLegend.get(lId,hex(lId))) ]
        for lBlock in lBlocks:
            lLine += [lBlock[lId],hex(lBlock[lId])] if lBlock[lId] is not None else ['fail']*2
        print( '|'.join(['']+[kCellFmt.format(lCell) for lCell in lLine]+['']))
    print ( '-'*lLineLen)
# ------------------------------------------------------------------------------
