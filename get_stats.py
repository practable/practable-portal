#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2023 Richard Hull and contributors
# See LICENSE.rst for details.

'''
It provides lookup of CPU, memory, disk utilization, temperature, IP address and system Uptime.

'''


import time
import traceback
import json

import serial
from pathlib import Path
from datetime import datetime
#from demo_opts import get_device
#from luma.core.render import canvas
#from luma.core.virtual import viewport
#from PIL import ImageFont
#from PIL import Image
#from PIL import ImageOps
import psutil
import subprocess as sp
import socket
from collections import OrderedDict
import uuid

PORT = "/dev/serial0"
BAUD = 115200

STR_LEN = 20




def pack_strings(strings):
    out = bytearray()
    for s in strings:
        b = s.encode("ascii")[:STR_LEN]
        b = b.ljust(STR_LEN, b'\0')
        out.extend(b)
    return out


def get_temp():
    temp = float(sp.getoutput("vcgencmd measure_temp").split("=")[1].split("'")[0])
    return f"{temp:.2f}"


def get_cpu():
    cpu = psutil.cpu_percent()
    return f"{cpu:.2f}"


def get_mem():
    mem =  psutil.virtual_memory().percent
    return f"{mem:.2f}"

def get_disk_usage():
    usage = psutil.disk_usage("/")
    percentage_used = (usage.used / usage.total * 100)
    formatted = f"{percentage_used:.2f}"
    return formatted


def get_uptime():
    uptime = ("%s" % (datetime.now() - datetime.fromtimestamp(psutil.boot_time()))).split(".")[0]
    return uptime


def find_single_ipv4_address(addrs):
    for addr in addrs:
        if addr.family == socket.AddressFamily.AF_INET:  # IPv4
            return addr.address


def get_ipv4_address(interface_name=None):
    if_addrs = psutil.net_if_addrs()

    if isinstance(interface_name, str) and interface_name in if_addrs:
        addrs = if_addrs.get(interface_name)
        address = find_single_ipv4_address(addrs)
        return address if isinstance(address, str) else ""
    else:
        if_stats = psutil.net_if_stats()
        # remove loopback
        if_stats_filtered = {key: if_stats[key] for key, stat in if_stats.items() if "loopback" not in stat.flags}
        # sort interfaces by
        # 1. Up/Down
        # 2. Duplex mode (full: 2, half: 1, unknown: 0)
        if_names_sorted = [stat[0] for stat in sorted(if_stats_filtered.items(), key=lambda x: (x[1].isup, x[1].duplex), reverse=True)]
        if_addrs_sorted = OrderedDict((key, if_addrs[key]) for key in if_names_sorted if key in if_addrs)

        for _, addrs in if_addrs_sorted.items():
            address = find_single_ipv4_address(addrs)
            if isinstance(address, str):
                return address

        return ""


def get_ip(network_interface_name):
    return "IP: %s" % (get_ipv4_address(network_interface_name))

def get_mac():
    return ':'.join(f'{(uuid.getnode() >> i) & 0xFF:02X}' for i in range(0, 48, 8)[::-1])

def format_percent(percent):
    return "%5.1f" % (percent)


stat_names = ["Temp: ", "CPU: ", "Mem: ", "Disk: ", "Uptime: ", "IP: ", "MAC: "]

def make_json(temp, cpu, mem, disk, uptime, ip, mac):
    return json.dumps({
        "temp": temp,
        "cpu": cpu,
        "mem":mem,
        "disk":disk,
        "uptime":uptime,
        "ip":ip,
        "mac":mac
    })

def get_stats():
    data = make_json(
        get_temp(),
        get_cpu(),
        get_mem(),
        get_disk_usage(),
        get_uptime(),
        get_ipv4_address(None),
        get_mac()
    )
    #obj = json.loads(data)
    #for k, v in obj.items():
    #    print(f"{k}: {v}")
    print(data)
    #string_array.append(f"{get_temp()} degC")
    #string_array.append(f"{get_cpu()} %")
    #string_array.append(f"{get_mem()} %")
    #string_array.append(f"{get_disk_usage()} %")
    #string_array.append(f"{get_uptime()}")
    #string_array.append(f"{get_ipv4_address(None)}")
    #string_array.append(f"{get_mac()}")
    #iteration = 0;
    print("\n\n")
    #for line in string_array:
    #    print(stat_names[iteration], line)
    #    iteration += 1
    return data


def portal():

    print("Opening Serial...")
    ser = serial.Serial(PORT, BAUD, timeout=1)
    print("Serial Opened")


    while (True):
        cmd = ser.read(4)
        if cmd == b'GET?':
            json_payload = get_stats()
            #payload = pack_strings(json_payload)
            #ser.write(payload)
            ser.write(json_payload.encode('ascii'))
            ser.write(b'\n')

        #print("Temp: ", get_temp(), " degC")
        #print("CPU: ", get_cpu(), " %")
        #print("Mem: ", get_mem(),  " %")
        #print("Disk: ", get_disk_usage(), " %")
        #print("Uptime: ", get_uptime())
        #print("IP: ",get_ipv4_address(None))
        #print("MAC: ", get_mac())
        time.sleep(0.01)



try:
    portal()
except KeyboardInterrupt:
    print("Exiting")
    raise
except Exception as e:
    traceback.print_exc()
    print(e)
