kSSPAddresses = {
    (0,5): range(0x20, 0x24), # APA1
    (0,6): range(0x24, 0x28), # APA2
    (0,1): range(0x28, 0x2c), # APA3
    (0,2): range(0x2c, 0x30), # APA4
    (0,4): range(0x30, 0x34), # APA5
    (0,3): range(0x34, 0x38), # APA6
}

kWibAddresses = {
    (0,5): range(0x45, 0x4a), # APA1
    (0,6): range(0x4a, 0x4f), # APA2
    (0,1): range(0x4f, 0x54), # APA3
    (0,2): range(0x54, 0x59), # APA4
    (0,4): range(0x59, 0x5e), # APA5
    (0,3): range(0x5e, 0x63), # APA6
}

kAddressToSlot = {}
for addrgrp in [kSSPAddresses, kWibAddresses]:
    for slot, addrlist in addrgrp.iteritems():
        kAddressToSlot.update({addr:slot for addr in addrlist})





