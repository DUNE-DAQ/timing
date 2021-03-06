import collections

kSPSClockInHz = int(50e6)
kEventSize = 6

kCommandIDs = collections.OrderedDict([

    ('TimeSync', 0x0),
    ('Echo', 0x1),
    ('SpillStart', 0x2),
    ('SpillStop', 0x3),
    ('RunStart', 0x4),
    ('RunStop', 0x5),
    ('WibCalib', 0x6),
    ('SSPCalib', 0x7),
    ('FakeTrig0', 0x8),
    ('FakeTrig1', 0x9),
    ('FakeTrig2', 0xa),
    ('FakeTrig3', 0xb),
    ('BeamTrig', 0xc),
    ('NoBeamTrig', 0xd),
    ('ExtFakeTrig', 0xe),
])

kCommandNames = { v:k for k,v in kCommandIDs.items() }


# -----------------------------------------------------------------------------
kBoardSim = 0x1
kBoardFMC = 0x0
kBoardPC059 = 0x2
kBoardMicrozed = 0x3
kBoardTLU = 0x4
# kBoardKC705 = 'kc705'
kBoardFIB = 0x5

kCarrierEnclustraA35 = 0x0
kCarrierKC705 = 0x1
kCarrierMicrozed = 0x2
kCarrierATFC = 0x3
kCarrierAFC = 0x4

kDesignMaster = 0x0
kDesignOuroborosSim = 0x1
kDesignOuroboros = 0x2
kDesignEndpoint = 0x4
kDesignFanout = 0x5
kDesignOverlord = 0x6
kDesignEndpoBICRT = 0x7
kDesignChronos = 0x8
kDesignBoreas = 0x9

kDesignNameMap = {
    kDesignMaster: 'master',
    kDesignOuroboros: 'ouroboros',
    kDesignOuroborosSim: 'ouroboros-sim',
    kDesignEndpoint: 'endpoint',
    kDesignFanout: 'fanout',
    kDesignOverlord: 'overlord',
    kDesignEndpoBICRT: 'endpoint-bi-crt',
    kDesignChronos: 'chronos',
    kDesignBoreas: 'boreas',
}

kBoardNamelMap = {
    kBoardSim: 'sim',
    kBoardFMC: 'fmc',
    kBoardPC059: 'pc059',
    kBoardMicrozed: 'microzed',
    kBoardTLU: 'tlu',
    kBoardFIB: 'fib',
}

kCarrierNamelMap = {
    kCarrierEnclustraA35: 'enclustra-a35',
    kCarrierKC705: 'kc705',
    kCarrierMicrozed: 'microzed',
    kCarrierATFC: 'atfc',
    kCarrierAFC: 'afc',
}

kLibrarySupportedBoards = {
    kBoardFMC, kBoardPC059, kBoardTLU, kBoardSim, kBoardFIB
}
# -----------------------------------------------------------------------------

# -----------------
kEpStates = collections.OrderedDict([
        ( 0x0, 'Standing by')                              , # 0b0000 when W_RST,  -- Starting state after reset
        ( 0x1, 'Waiting for SFP signal')                   , # 0b0001 when W_LINK, -- Waiting for SFP LOS to go low
        ( 0x2, 'Waiting for good frequency check')         , # 0b0010 when W_FREQ, -- Waiting for good frequency check
        ( 0x3, 'Waiting for phase adjustment to complete') , # 0b0011 when W_ADJUST, -- Waiting for phase adjustment to complete
        ( 0x4, 'Waiting for comma alignment')              , # 0b0100 when W_ALIGN, -- Waiting for comma alignment, stable 50MHz phase
        ( 0x5, 'Waiting for 8b10 decoder good packet')     , # 0b0101 when W_LOCK, -- Waiting for 8b10 decoder good packet
        ( 0x6, 'Waiting for phase adjustment command')     , # 0b0110 when W_PHASE, -- Waiting for phase adjustment command
        ( 0x7, 'Waiting for time stamp initialisation')    , # 0b0111 when W_RDY, -- Waiting for time stamp initialisation
        ( 0x8, 'Ready')                                    , # 0b1000 when RUN, -- Good to go
        ( 0xc, 'Error in Rx')                              , # 0b1100 when ERR_R, -- Error in rx
        ( 0xd, 'Error in time stamp check')                , # 0b1101 when ERR_T, -- Error in time stamp check
        ( 0xe, 'Error in physical layer after lock')       , # 0b1110 when ERR_P, -- Physical layer error after lock)
    ])

def fmtEpState(aState):
    aState = aState.value()
    return '{} ({})'.format(kEpStates[aState], hex(aState)) if aState in kEpStates else hex(aState)
# -----------------
