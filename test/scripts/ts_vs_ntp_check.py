#!/usr/bin/env python

import time
import datetime
from datetime import timezone
import uhal
from timing.common.definitions import kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignOverlord
from timing.cli.toolbox import *

# constants
year_seconds = 0X1E13380
day_seconds = 0X15180
hour_seconds = 0XE10
minute_seconds = 0X3C
n_leap_years = 7 # 2000, 2004, 2008, 2012, 2016, 2020, 2024.
seconds_from_unix_to_gps_epoch = 946684800 #gps epoch 01/01/2000
dts_clock_freq = 62500000

uhal.setLogLevelTo(uhal.LogLevel.ERROR)
manager = uhal.ConnectionManager("file://../connections.xml")
mst_device = manager.getDevice("GIB")
mst=mst_device.getNode('master')
irig=mst_device.getNode('irig_time_source')

t_before_call_time = time.time()

irig_year = irig.getNode(f"csr.irig_date.year").read()
irig_day = irig.getNode(f"csr.irig_date.day").read()
irig_hour = irig.getNode(f"csr.irig_time.hour").read()
irig_minute = irig.getNode(f"csr.irig_time.minute").read()
irig_sececond = irig.getNode(f"csr.irig_time.second").read()

irig_pps = irig.getNode(f"pps_ctr").read()
irig_secs_since_epoch_raw = irig.getNode(f"seconds_since_epoch").readBlock(2)
irig_ts_raw = irig.getNode(f"tstamp").readBlock(2)

ts_raw=mst.getNode("tstamp.ctr").readBlock(2)
mst_device.dispatch()

t_after_call_time = time.time()

irig_secs_since_epoch_calc = irig_year*year_seconds + (irig_day-1)*day_seconds \
                            + irig_hour*hour_seconds + irig_minute*minute_seconds + irig_sececond \
                            + n_leap_years*day_seconds \
                            + seconds_from_unix_to_gps_epoch

irig_ts = tstamp2int(irig_ts_raw)
ts = tstamp2int(ts_raw)
irig_secs_since_epoch = tstamp2int(irig_secs_since_epoch_raw)

ts_s = ts / dts_clock_freq
irig_ts_s = irig_ts / dts_clock_freq
ts_time = datetime.datetime.fromtimestamp(ts_s)
ts_time_human = ts_time.strftime('%Y-%m-%d %H:%M:%S.%f')

print(f"time before call          {t_before_call_time}")
print(f"DTS ts (mst)  in seconds  {ts_s}")
print(f"DTS ts (irig) in seconds  {irig_ts_s}")
print(f"IRIG secs from epoch      {irig_secs_since_epoch}")
print(f"IRIG secs from epoch calc {irig_secs_since_epoch_calc}")
print(f"time after call           {t_after_call_time}")
print("")
print(f"IRIG year      {irig_year}")
print(f"IRIG day(-1)   {irig_day-1}")
print(f"IRIG hour      {irig_hour}")
print(f"IRIG minute    {irig_minute}")
print(f"IRIG second    {irig_sececond}")
print(f"now            {datetime.datetime.now(timezone.utc)}")
print("")
print(f"IRIG time (+1s) ticks  {(irig_secs_since_epoch+1)*dts_clock_freq}")
print(f"DTS ts (irig) ticks    {irig_ts}")