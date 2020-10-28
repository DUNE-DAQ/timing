#!/usr/bin/python

import sys
import uhal
import time

uhal.setLogLevelTo(uhal.LogLevel.NOTICE)
manager = uhal.ConnectionManager("file://connections.xml")
hw_list = [manager.getDevice(i) for i in sys.argv[1:]]

if len(hw_list) == 0:
    print "No targets specified - I'm done"
    sys.exit()

for hw in hw_list:
   hw.getNode("master.global.csr.ctrl.part_sel").write(0)
   hw.getNode("master.partition.csr.ctrl.run_req").write(1)
   hw.dispatch()

   hw.getNode("master.partition.csr.ctrl.trig_mask").write(0xff)
   hw.getNode("master.partition.csr.ctrl.trig_en").write(1)
   hw.getNode("master.partition.csr.ctrl.buf_en").write(1)
   hw.dispatch()

   hw.getNode("master.scmd_gen.chan_ctrl.en").write(1)
   hw.getNode("master.scmd_gen.chan_ctrl.type").write(0x8)
   hw.getNode("master.scmd_gen.ctrl.en").write(0)
   hw.dispatch()