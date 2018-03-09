
import re

import definitions as defs
from click import echo, style, secho

# ------------------------------------------------------------------------------
def sanitizeConnectionPaths(aConnectionPaths):

    lConnectionList = aConnectionPaths.split(';')
    for i,c in enumerate(lConnectionList):
        if re.match('^\w+://.*', c) is None:
            lConnectionList[i] = 'file://'+c
    return ';'.join(lConnectionList)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def readSubNodes(aNode):
    lValues = { n:aNode.getNode(n).read() for n in aNode.getNodes() }
    aNode.getClient().dispatch()
    return lValues
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def resetSubNodes(aNode, aValue=0x0):
    """
    Reset subnodes of aNode to aValue
    """
    lValues = { n:aNode.getNode(n).write(aValue) for n in aNode.getNodes() }
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


# ------------------------------------------------------------------------------
def printCounters( aTopNode, aSubNodes ):

    lNumCtrs = 0x10
    lBlocks = []
    # lBlocks = [ aTopNode.getNode(lKey).readBlock(lNumCtrs) for lKey in aSubNodes ]

    # try:
    #     aTopNode.getClient().dispatch()
    # except uhal.exception as e:
    #     for b in lBlocks:
    #         print (b.valid())
    #     raise SystemExit(0)
    
    for lKey in aSubNodes:
        try:
            ctrs = aTopNode.getNode(lKey).readBlock(lNumCtrs)
            aTopNode.getClient().dispatch()
            lBlocks.append(ctrs.value())

        except uhal.exception as e:
            print ('Failed to read ', lKey)
            lBlocks.append([None]*lNumCtrs)

    # Just a bit of math
    lCellWidth = 10
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
    lLine = ['Cmd'] +( ['cnts', 'hex' ]*len(aSubNodes) )
    lHdr = '|'.join(['']+[kCellFmt.format(lCell) for lCell in lLine]+[''])
    print ( '-'*lLineLen)
    print ( lHdr )
    print ( '-'*lLineLen)

    for lId in xrange(lNumCtrs):

        lLine = [ (defs.kCommandNames.get(lId,hex(lId))) ]
        for lBlock in lBlocks:
            lLine += [lBlock[lId],hex(lBlock[lId])] if lBlock[lId] is not None else ['fail']*2
        print( '|'.join(['']+[kCellFmt.format(lCell) for lCell in lLine]+['']))
    print ( '-'*lLineLen)
# -----------------
