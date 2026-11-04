#!/usr/bin/env python

import sys
import uhal
import time
import argparse

import timing
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignBoreas

# size of one MMCM phase adjust step [ps]
phase_step=17.857

def perform_phase_scan(master_device, endpoint_address, step_size, pause_after_step):
    print("Starting phase scan")

    mst=master_device.getNode('master')

    # 16000/17.857 [ps] gives 896 steps
    for fine_phase_setting in range(0,897,step_size):

        mst.apply_endpoint_delay(endpoint_address, 0, fine_phase_setting)
        time.sleep(0.01)
        mst.setup_endpoint_rx(endpoint_address)

        ept_state_before_deskew_done = mst.read_endpoint_state(endpoint_address)
        if (ept_state_before_deskew_done < 6 or ept_state_before_deskew_done > 7) :    
            raise RuntimeError(f"bad ept state after delay set: {ept_state_before_deskew_done}")

        mst.set_endpoint_deskew_done(endpoint_address)

        ept_state_after_deskew_done = mst.read_endpoint_state(endpoint_address)
        if (ept_state_after_deskew_done < 6 or ept_state_after_deskew_done > 8) :    
            raise RuntimeError(f"bad ept state after deskew done: {ept_state_after_deskew_done}")

        rtt = mst.measure_endpoint_rtt(endpoint_address, False)
        print(f"phase: ~ {fine_phase_setting*phase_step:.2f} ps, state: {ept_state_after_deskew_done}, rtt: {rtt}")

        if pause_after_step:
            input("Press a key to continue...")

        mst.switch_endpoint_sfp(endpoint_address, False)

    print("Phase scan complete")

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('connections_file_path')
    parser.add_argument('master_device_name')
    parser.add_argument('endpoint_address', type=int)
    parser.add_argument('-s', '--step-size', nargs='?', type=int, default=1)
    parser.add_argument('-p', '--pause-after-step', action='store_true')
    parser.add_argument('-n', '--scans-number', type=int, default=1)
    args = parser.parse_args()

    print(f"Using master {args.master_device_name} from {args.connections_file_path}")
    print(f"Scanning phase settings for endpoint address {args.endpoint_address}")
    print(f"Using scan step of {phase_step*args.step_size} ps")
    n_scans_str= f"{args.scans_number}" if args.scans_number > 0 else "infinite"
    print(f"I will perform {n_scans_str} number of scans")

    print("")

    uhal.setLogLevelTo(uhal.LogLevel.ERROR)
    manager = uhal.ConnectionManager("file://"+args.connections_file_path)
    master_device = manager.getDevice(args.master_device_name).getNode('')

    n_scans_performed=0
    while True:
        if args.scans_number > 0 and n_scans_performed >= args.scans_number:
            break
        perform_phase_scan(master_device, args.endpoint_address, args.step_size, args.pause_after_step)
        n_scans_performed=n_scans_performed+1