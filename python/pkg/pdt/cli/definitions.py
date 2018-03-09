import collections


kCommandIDs = collections.OrderedDict([

    ('TimeSync', 0x0),
    ('Echo', 0x1),
    ('SpillStart', 0x2),
    ('SpillStop', 0x3),
    ('RunStart', 0x4),
    ('RunStop', 0x5),
    ('FakeTrig', 0x8),
])

kCommandNames = { v:k for k,v in kCommandIDs.iteritems() }