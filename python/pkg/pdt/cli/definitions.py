import collections


kCommandIDs = collections.OrderedDict([

    ('TimeSync', 0x0),
    ('Echo', 0x1),
    ('SpillStart', 0x2),
    ('SpillStop', 0x3),
    ('RunStart', 0x4),
    ('RunStop', 0x5),
    ('FakeTrig0', 0x8),
    ('FakeTrig1', 0x9),
    ('FakeTrig2', 0xa),
    ('FakeTrig3', 0xb),
])

kCommandNames = { v:k for k,v in kCommandIDs.iteritems() }


# -----------------------------------------------------------------------------
kBoardSim = 0x1
kBoardFMC = 0x0
kBoardPC059 = 0x2
kBoardMicrozed = 0x3
kBoardTLU = 0x4
# kBoardKC705 = 'kc705'

kCarrierEnclustraA35 = 0x0
kCarrierKC705 = 0x1
kCarrierMicrozed = 0x2


kBoardNamelMap = {
    kBoardSim: 'sim',
    kBoardFMC: 'fmc',
    kBoardPC059: 'pc059',
    kBoardMicrozed: 'microzed',
    kBoardTLU: 'tlu'
}

kCarrierNamelMap = {
    kCarrierEnclustraA35: 'enclustra-a35',
    kCarrierKC705: 'kc705',
    kCarrierMicrozed: 'microzed',
}
# -----------------------------------------------------------------------------
