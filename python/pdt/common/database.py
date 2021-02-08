import collections
import itertools

kSSPAddresses = {
    (0,5): list(range(0x20, 0x24)), # APA1 ssp101 to 104
    (0,6): list(range(0x24, 0x28)), # APA2
    (0,1): list(range(0x28, 0x2c)), # APA3
    (0,2): list(range(0x2c, 0x30)), # APA4
    (0,4): list(range(0x30, 0x34)), # APA5
    (0,3): list(range(0x34, 0x38)), # APA6
}

kWibAddresses = {
    (0,5): list(range(0x45, 0x4a)), # APA1
    (0,6): list(range(0x4a, 0x4f)), # APA2
    (0,1): list(range(0x4f, 0x54)), # APA3
    (0,2): list(range(0x54, 0x59)), # APA4
    (0,4): list(range(0x59, 0x5e)), # APA5
    (0,3): list(range(0x5e, 0x63)), # APA6
}

kAddressToSlot = {}
for addrgrp in [kSSPAddresses, kWibAddresses]:
    for slot, addrlist in addrgrp.items():
        kAddressToSlot.update({addr:slot for addr in addrlist})




"""
Entry:
common name, taddr, fanout, slot
'ssp101': (0x20, 0, 5)


"""

def __genRange( aNum, aGrp, aBaseName, aBaseId, aBaseTAddr, aFanout, aSlot ):
    return { '{}{}'.format(aBaseName, aBaseId+i+1):(aGrp, aBaseTAddr+i, aFanout, aSlot) for i in range(0,aNum) }


__db__ = {}

# Add the SSPs
__db__.update(__genRange(4, 'ssp', 'ssp', 100, 0x20, 0, 5) )
__db__.update(__genRange(4, 'ssp', 'ssp', 200, 0x24, 0, 6) )
__db__.update(__genRange(4, 'ssp', 'ssp', 300, 0x28, 0, 1) )
__db__.update(__genRange(4, 'ssp', 'ssp', 400, 0x2c, 0, 2) )
__db__.update(__genRange(4, 'ssp', 'ssp', 500, 0x30, 0, 4) )
__db__.update(__genRange(4, 'ssp', 'ssp', 600, 0x34, 0, 3) )

# Add the WIBs
__db__.update(__genRange(5, 'wib', 'wib', 100, 0x45, 0, 5) )
__db__.update(__genRange(5, 'wib', 'wib', 200, 0x4a, 0, 6) )
__db__.update(__genRange(5, 'wib', 'wib', 300, 0x4f, 0, 1) )
__db__.update(__genRange(5, 'wib', 'wib', 400, 0x54, 0, 2) )
__db__.update(__genRange(5, 'wib', 'wib', 500, 0x59, 0, 4) )
__db__.update(__genRange(5, 'wib', 'wib', 600, 0x5e, 0, 3) )

# Vst
__db__['ssp003'] = ('vst', 0xc0, 0, 7)

def getDataBase():
    return __db__


# for x in sorted(__db__):
#     print x,__db__[x]

# for x in itertools.chain.from_iterable(__db__.itervalues()):
    # print x
# ------------------------------------------------------------------------------
def findByUId( uid ):
    entry = __db__.get(uid, None)
    return (uid,)+entry if entry is not None else None
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def findByTAddr( taddr ):
    uid = next( ( k for k,(_, ta, _, _) in __db__.items() if taddr == ta ), None )

    return findByUId(uid) if uid is not None else None
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def findByGrp( aGrp ):
    return [ (k,)+(grp, taddr, fanout, slot) for k,(grp, taddr, fanout, slot) in __db__.items() if grp == aGrp]
# ------------------------------------------------------------------------------

