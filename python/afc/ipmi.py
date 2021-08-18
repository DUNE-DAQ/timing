from __future__ import print_function

import pyipmi.interfaces
import click
import struct

# ------------------------------------------------------------------------------
def establishIPMIConnectionToAMC(mch_ip_adr,amc_slot):
    amc_ipmb_addresses= [0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e, 0x80, 0x82, 0x84, 0x86, 0x88]
    amc_ipmb_adr = amc_ipmb_addresses[amc_slot-1]
    
    interface = pyipmi.interfaces.create_interface('ipmitool', interface_type='lan')
    connection = pyipmi.create_connection(interface)

    connection.target = pyipmi.Target(amc_ipmb_adr)
    connection.target.set_routing([(0x81,0x20,0),(0x20,0x82,7),(0x20,amc_ipmb_adr,None)])
    connection.session.set_session_type_rmcp(mch_ip_adr, port=623)
    connection.session.set_auth_type_user('', '')
    connection.session.establish()
    return connection
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def readRegOverIPMI(ipmi_connection, reg):
    raw_read_cmd = b'\x00\x02\x4B\x01\x01'
    cmd = raw_read_cmd+struct.pack("B", reg)
    
    max_attempts=10
    read_attempts=0

    while True:
        if read_attempts > max_attempts:
            raise click.ClickException("Failed to read value of reg {} after {} attempts".format(hex(reg), max_attempts))
        read_cmd_result = []
        result = ipmi_connection.raw_command(0x00, 0x30, cmd)
        for char in result:
            read_cmd_result.append(char)
        if read_cmd_result[1] == 1 and read_cmd_result[2] == 1:
            return read_cmd_result[3]
        else:
            read_attempts += 1
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def writeRegOverIPMI(ipmi_connection, reg, data):
    raw_write_cmd = b'\x00\x02\x4B\x02\x01'
    cmd = raw_write_cmd+struct.pack("B", reg)+struct.pack("B", data)
    
    max_attempts=10
    write_attempts=0

    while True:
        if write_attempts > max_attempts:
            raise click.ClickException("Failed to write value of reg {} after {} attempts".format(hex(reg), max_attempts))
        write_cmd_result = []
        result = ipmi_connection.raw_command(0x00, 0x30, cmd)
        for char in result:
            write_cmd_result.append(char)
        if write_cmd_result[1] == 2 and write_cmd_result[2] == 1:
            return
        else:
            write_attempts += 1
# ------------------------------------------------------------------------------