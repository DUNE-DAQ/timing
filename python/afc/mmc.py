from __future__ import print_function

import struct
import click
from click import echo, style, secho
from .click_texttable import Texttable
from . import toolbox
from . import ipmi

# ------------------------------------------------------------------------------
@click.group('mmc', invoke_without_command=False)
@click.pass_obj
def mmc(obj):
    # make ipmi connection
    obj.ipmi_connection = ipmi.establishIPMIConnectionToAMC(obj.mch_ip_adr, obj.amc_slot)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@mmc.command('read-pin-port-status', short_help='Read back the configuration and states of the AMC MMC pins for a particular port')
@click.option('--port-number', 'port_number', required=True, type=toolbox.IntRange(0x0,0x4), help='MMC pin port, 0-4')
@click.pass_obj
def read_pin_port_status(obj, port_number):
    echo( readGPIOPortOverIPMI(obj.ipmi_connection, port_number) )
    obj.ipmi_connection.session.close()
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
def readGPIOPortOverIPMI(ipmi_connection, port):
    raw_gpio_cmd = b'\x01'
    mode=b'\x00'
    cmd = raw_gpio_cmd+struct.pack("B", port)+mode
    
    cmd_result = []
    result = ipmi_connection.raw_command(0x00, 0x30, cmd)

    for char in result:
        cmd_result.append(char)
    
    # first 4 bytes contain the pin directions    
    port_directions_flag=0b0
    for i in range(4,0,-1):
        port_directions_flag = (port_directions_flag << 8) | cmd_result[i]

    # second 4 bytes contain the pin states
    port_states_flag=0b0
    for i in range(8,4,-1):
        port_states_flag = (port_states_flag << 8) | cmd_result[i]

    portTable = Texttable(max_width=0)
    portTable.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
    portTable.set_cols_align(["l", "l", "l"])
    portTable.set_chars(['-', '|', '+', '-'])
    portTable.header( ['Pin', 'Direction', 'State'] )

    for i in range(32):
        pin_dir = 'In'
        pin_dir_colour = 'yellow'
        if port_directions_flag & (0x1 << i):
            pin_dir = 'Out'
            pin_dir_colour = 'green'
        
        pin_state = 'Low'
        pin_state_colour = 'blue'
        if port_states_flag & (0x1 << i):
            pin_state = 'High'
            pin_state_colour = 'red'

        portTable.add_row( [i, style(pin_dir, fg=pin_dir_colour), style(pin_state, fg=pin_state_colour)] )
    
    print("Port {} pins".format(port))
    return portTable.draw()
# ------------------------------------------------------------------------------

# mode description: 0 - read port info, 1 - set port dir to in, 2 - set port dir to out with optional value
# ------------------------------------------------------------------------------
def configureGPIOPortOverIPMI(ipmi_connection, port, mode, pin, value=-1):
    
    if mode < 1 or mode > 2:
        raise click.ClickException("Valid configuring modes are 1 or 2")

    raw_gpio_cmd = b'\x01'
    cmd = raw_gpio_cmd + struct.pack("B", port) + struct.pack("B", mode) + struct.pack("B", pin)
    
    if mode==2 and value >= 0:
        cmd = cmd+struct.pack("B", value)

    cmd_attempts=0
    max_attempts=10
    while True:
        if cmd_attempts > max_attempts:
            raise click.ClickException("Failed to configure port {} pin {} after {} attempts".format(hex(port), hex(pin), max_attempts))
        cmd_result = []
        result = ipmi_connection.raw_command(0x00, 0x30, cmd)
        for char in result:
            cmd_result.append(char)
        if cmd_result[1] == 0 and mode == 0x1:
            return
        elif cmd_result[1] == 1 and mode == 0x2:
            if value < 0:
                return
            else:
                cmd_result[2] == value
                return
        else:
            read_attempts += 1
# ------------------------------------------------------------------------------