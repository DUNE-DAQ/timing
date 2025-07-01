#!/usr/bin/env python

import sys
import os
import h5py
import array

import detdataformats
from detdataformats import HSIFrame

def main(raw_file: str):

    f = h5py.File(raw_file, 'r')
    hsi_dataset=f['hsi_frames']
    print(f"n events: {hsi_dataset.shape[0]}")

    last_sequence_counter=-1
    last_ts=0
    for i in range(hsi_dataset.shape[0]):
        #print(hsi_dataset[i])
        hsi_array=array.array('I', hsi_dataset[i])
        hsi_frame = HSIFrame(hsi_array.tobytes())
        sequence_counter=hsi_frame.sequence
        ts=hsi_frame.get_timestamp()
        delta_ts=ts-last_ts
        if last_ts == 0:
            delta_ts=0
        data = hsi_frame.input_low | (hsi_frame.input_high << 32)
        print(f"event: {sequence_counter} ts: {ts} signals {data} delta ts: {delta_ts}")
        if (last_sequence_counter+1 != sequence_counter and last_sequence_counter != -1):
            print("[WARNING] break in sequence")
        last_sequence_counter=sequence_counter
        last_ts=ts
        
if __name__ == '__main__':

    raw_file=os.path.abspath(sys.argv[1])
    print(f"data file: {raw_file}")
    main(raw_file)