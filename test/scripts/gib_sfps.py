#!/usr/bin/env python

import time
import datetime
from datetime import timezone
import uhal
# from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignOverlord
from timing.cli.toolbox import *

uhal.setLogLevelTo(uhal.LogLevel.ERROR)
manager = uhal.ConnectionManager("file:///home/wx21978/projects/timing/fib-slot-tests/conn.xml")
gib_device = manager.getDevice("GIB_V3")
lIO=gib_device.getNode('io')

gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x0)
gib_device.dispatch()
gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x1)
gib_device.dispatch()
test_id = 1
print(type(lIO))
try:
    print(lIO.get_sfp_status(test_id))
except Exception as e:
    print(f"SFP {test_id} failed with: {e}")
try:
    print(lIO.get_sfp_status(test_id))
except Exception as e:
    print(f"SFP {test_id} failed with: {e}")
gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x0)
gib_device.dispatch()
gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x1)
gib_device.dispatch()
try:
    print(lIO.get_sfp_status(test_id))
except Exception as e:
    print(type(e))
    if isinstance(e, RuntimeError) and str(e) == " I2C bus: i2c error. Transfer finished but bus still busy I2CException on bus: i2c":
        print(f"Correct RuntimeError -- {e}")
    else:
        raise e

try:
    print(lIO.get_sfp_status(test_id))
except RuntimeError as e:
    if str(e) == " I2C bus: i2c error. Transfer finished but bus still busy I2CException on bus: i2c":
        print(f"Correct RuntimeError -- {e}")
    else:
        gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x0)
        gib_device.dispatch()
        gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x1)
        gib_device.dispatch()
        raise e

gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x0)
gib_device.dispatch()
gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x1)
gib_device.dispatch()
print(lIO.get_sfp_status(test_id))

for sfp_id in range(7):
    gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x0)
    gib_device.dispatch()
    gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x1)
    gib_device.dispatch()
    try:
        print(lIO.get_sfp_status(sfp_id))
    except Exception as e:
        print(f"SFP {sfp_id} failed with: {e}")

# gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x0)
# gib_device.dispatch()
# gib_device.getNode("io.csr.ctrl.i2c_sw_rst").write(0x1)
# gib_device.dispatch()
# for sfp_id in range(7):
#     try:
#         print(lIO.get_sfp_status(sfp_id))
#     except Exception as e:
#         print(f"SFP {sfp_id} failed with: {e}")
