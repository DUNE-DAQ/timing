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