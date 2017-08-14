
import re

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
    lValues = { n:aNode.getNode(n).write(aValue) for n in aNode.getNodes() }
    aNode.getClient().dispatch()
# ------------------------------------------------------------------------------
