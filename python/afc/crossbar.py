from __future__ import print_function

import click
from click import echo, style, secho
from .click_texttable import Texttable
from . import toolbox
from . import ipmi

# ------------------------------------------------------------------------------
@click.group('crossbar', invoke_without_command=False)
@click.pass_obj
def crossbar(obj):
    # make ipmi connecfon
    obj.ipmi_connection = ipmi.establishIPMIConnectionToAMC(obj.mch_ip_adr, obj.amc_slot)
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
@crossbar.command('configure', short_help='Configures the clock cross-bar on the AFC')
@click.option('--tx-enable-flag', 'tx_enable_flag', required=False, type=toolbox.IntRange(0x0,0xffff), default=0b0000000000100000, help='Output tx enable flags')
@click.option('--xpt-map0', 'xpt_map0', required=False, type=str, default="77777d7777777777", help='Input->output crossbar map 0')
@click.option('--xpt-map1', 'xpt_map1', required=False, type=str, default="7777777777777777", help='Input->output crossbar map 1')
@click.option('--active-xpt-map', 'active_xpt_map', required=False, type=toolbox.IntRange(0x0,0x1), default=0x0, help='Active xpt map')
@click.pass_obj
def configure(obj, tx_enable_flag, xpt_map0, xpt_map1, active_xpt_map):

    if (len(xpt_map0) != 16 or len(xpt_map1) != 16):
        raise click.ClickException("XPT map length must be 16. Lenghts of provided maps are, 0: {}, 1: {}".format(len(xpt_map0), len(xpt_map1)))

    # reset clock cross-bar
    ipmi.writeRegOverIPMI(obj.ipmi_connection, 0x0, 0x01)

    # input->output map in output order of 0 1 2 3 4 5 6 7 8 9 10 11  12 13 14 15
    applyCrossbarXPTMapConfig(obj.ipmi_connection, xpt_map0, 0)
    applyCrossbarXPTMapConfig(obj.ipmi_connection, xpt_map1, 1)
    
    # select which map is active
    ipmi.writeRegOverIPMI(obj.ipmi_connection, 0x81, active_xpt_map)

    #enable new config
    ipmi.writeRegOverIPMI(obj.ipmi_connection, 0x80, 0x01)

    # output tx enable flags in order of 15 -> 0
    applyCrossbarTxConfig(obj.ipmi_connection, tx_enable_flag)

    print("Crossbar config applied")
    obj.ipmi_connection.session.close()
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
@crossbar.command('read-config', short_help='Reads the config of the clock cross-bar on the AFC')
@click.pass_obj
def read_config(obj):
    
    xpt_map_0 = readCrossbarXPTMapConfig(obj.ipmi_connection, 0)
    xpt_map_1 = readCrossbarXPTMapConfig(obj.ipmi_connection, 1)
    tx_states = readCrossbarTxConfig(obj.ipmi_connection)
    
    active_map = ipmi.readRegOverIPMI(obj.ipmi_connection, 0x81)
    print("Active map: ",active_map)
    echo  ( formatCrossbarConfigTable(xpt_map_0, xpt_map_1, tx_states,active_map) )
    obj.ipmi_connection.session.close()
# ------------------------------------------------------------------------------ 


# ------------------------------------------------------------------------------
def applyCrossbarTxConfig(ipmi_connection, tx_enable_flag):
    tx_control_ctrl_reg_start = 0x20
    
    for i in range(16):
        reg_adr = tx_control_ctrl_reg_start+i 

        # TX Basic Control Register flags:
        # [6] TX CTL SELECT - 0: PE and output level control is derived from common lookup table
        #                     1: PE and output level control is derived from per port drive control registers
        # [5:4] TX EN[1:0]  - 00: TX disabled, lowest power state
        #                     01: TX standby
        #                     10: TX squelched
        #                     11: TX enabled
        # [3] Reserved      - Set to 0
        # [2:1] PE[2:0]     - If TX CTL SELECT = 0,
        #                       000: Table Entry 0
        #                       001: Table Entry 1
        #                       010: Table Entry 2
        #                       011: Table Entry 3
        #                       100: Table Entry 4
        #                       101: Table Entry 5
        #                       110: Table Entry 6
        #                       111: Table Entry 7
        #                   - If TX CTL SELECT = 1, PE[2:0] are ignored
        tx_state = 0b0110000 if tx_enable_flag & (1 << i) else 0b0000000
        #print("tx state for output {} at adr {}: ".format(i,hex(reg_adr)) +hex(tx_state))
        ipmi.writeRegOverIPMI(ipmi_connection, reg_adr, tx_state)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def applyCrossbarXPTMapConfig(ipmi_connection, xpt_map, map_number):
    xpt_reg_values=[]
    for i in range(2,18,2):
        map_value_str = xpt_map[i-2:i]
        map_value=int(map_value_str, 16)
        reg_value_lo = int('{:08b}'.format(map_value)[4:], 2) << 4
        reg_value_hi = int('{:08b}'.format(map_value)[:4], 2)
        reg_value = reg_value_lo | reg_value_hi
        xpt_reg_values.append(reg_value)

    xpt_map_reg_adrs_start=[0x90, 0x98]

    for i in range(len(xpt_reg_values)):
        reg_adr = xpt_map_reg_adrs_start[map_number]+i
        reg_value = xpt_reg_values[i]
        ipmi.writeRegOverIPMI(ipmi_connection, reg_adr, reg_value)
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def readCrossbarXPTMapConfig(ipmi_connection, map_number):
    xpt_map=[]

    xpt_map_reg_adrs_start=[0x90, 0x98]

    for i in range(8):
        reg_adr = xpt_map_reg_adrs_start[map_number]+i
        reg_value = ipmi.readRegOverIPMI(ipmi_connection, reg_adr)
        reg_value_lo = reg_value & 0x0f
        reg_value_hi = (reg_value >> 4) & 0x0f
        xpt_map.append(reg_value_lo)
        xpt_map.append(reg_value_hi)
    return xpt_map
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def readCrossbarTxConfig(ipmi_connection):
    tx_states=[]

    tx_control_ctrl_reg_start = 0x20

    for i in range(16):
        reg_adr = tx_control_ctrl_reg_start+i
        reg_value = ipmi.readRegOverIPMI(ipmi_connection, reg_adr)
        tx_states.append(reg_value)
    return tx_states
# ------------------------------------------------------------------------------


# ------------------------------------------------------------------------------
def formatCrossbarConfigTable(map_0, map_1, tx, active_map):

    map_0_colour='green'
    map_1_colour='white'
    if active_map == 1:
        map_0_colour='white'
        map_1_colour='green'

    configTable = Texttable(max_width=0)
    configTable.set_deco(Texttable.VLINES | Texttable.BORDER | Texttable.HEADER)
    configTable.set_cols_align(["l", "l", "l", "l"])
    configTable.set_chars(['-', '|', '+', '-'])
    configTable.header( ['Output', style('Map 0', fg=map_0_colour), style('Map 1', fg=map_1_colour), 'Tx state'] )

    for i in range(len(map_0)):
        tx_state = (tx[i] & 0b0110000) >> 4 
        tx_state_names = ["Disabled", "Standby", "Squelched", "Enabled"]
        tx_state_colours=['red', 'white', 'blue', 'green']
        configTable.add_row( [i, style(str(map_0[i]), fg=map_0_colour), style(str(map_1[i]), fg=map_1_colour), style(tx_state_names[tx_state], fg=tx_state_colours[tx_state])] )
        
    return configTable.draw()
# ------------------------------------------------------------------------------