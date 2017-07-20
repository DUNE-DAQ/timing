#!/usr/bin/env python
import uhal

def readMAC( device ):
	  addr1 = uhal.readIPbusConfigurationSpace(device.getClient(), 0x1)
	  addr2 = uhal.readIPbusConfigurationSpace(device.getClient(), 0x2)
	  device.dispatch()
	  mac = "%012x" % ( ((0xffff & addr2) << 32) | addr1 )
	  print mac
	  print device.id(), device.uri()
	  print device.id(), 'mac:',':'.join([mac[i]+mac[i+1] for i in range(0,12,2)])


def getinfo():
	uhal.setLogLevelTo(uhal.LogLevel.WARNING)
	
	cm = uhal.ConnectionManager('file://${PDT_TESTS}/etc/connections.xml')

	device = cm.getDevice('DUNE_FMC_SLAVE_TUN')

	readMAC(device)

if __name__ == '__main__':
	getinfo()