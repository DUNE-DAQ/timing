import time
import uhal
from I2CuHal import I2CCore
import StringIO
import csv
import logging

class si5344:
    #Class to configure the Si5344 clock generator
    log = logging.getLogger('si5344')

    def __init__(self, i2c, slaveaddr=0x68):
        self.i2c = i2c
        self.slaveaddr = slaveaddr
        #self.configList=

    #def writeReg(self, address, data):

    def readRegister(self, aRegAddr, aNwords):
        ### Read a specific register on the Si5344 chip. There is not check on the validity of the address but
        ### the code sets the correct page before reading.

        #First make sure we are on the correct page
        currentPg= self.getPage()
        requirePg= (aRegAddr & 0xFF00) >> 8
#        print "REG", hex(aRegAddr), "CURR PG" , hex(currentPg[0]), "REQ PG", hex(requirePg)
        if currentPg[0] != requirePg:
            self.setPage(requirePg)
        #Now read from register.
        addr=[]
        addr.append(aRegAddr)
        mystop=True
        self.i2c.write( self.slaveaddr, addr, mystop)
        # time.sleep(0.1)
        res = self.i2c.read( self.slaveaddr, aNwords)
        return res

    def writeRegister(self, aRegAddr, aData, verbose=False):
        ### Write a specific register on the Si5344 chip. There is not check on the validity of the address but
        ### the code sets the correct page before reading.
        ### aRegAddr is an int
        ### aData is a list of ints

        #First make sure we are on the correct page
        aRegAddr= aRegAddr & 0xFFFF
        currentPg= self.getPage()
        requirePg= (aRegAddr & 0xFF00) >> 8
#        print "REG", hex(aRegAddr), "CURR PG" , currentPg, "REQ PG", hex(requirePg)
        if currentPg[0] != requirePg:
            self.setPage(requirePg)
        #Now write to register.
        aData.insert(0, aRegAddr)
        if verbose:
            print "  Writing (page %d): " % requirePg
            result="\t  "
            for iaddr in aData:
                result+="%#02x "%(iaddr)
            print result
        nwrt = self.i2c.write(self.slaveaddr, aData)
        assert(nwrt == len(aData))

        #time.sleep(0.01)

    def setPage(self, aPage, verbose=False):
        #Configure the chip to perform operations on the specified address page.
        mystop=True
        lData= [0x01, aPage]
        self.i2c.write( self.slaveaddr, lData, mystop)
        #time.sleep(0.01)
        logging.debug("Si5344 Set Reg Page: 0x%x", aPage)


    def getPage(self, verbose=False):
        #Read the current address page
        mystop = True
        lData =  [0x01]
        self.i2c.write( self.slaveaddr, lData, mystop)
        rPage= self.i2c.read( self.slaveaddr, 1)
        #time.sleep(0.1)
        logging.debug("Page read: 0x%x", rPage[0])
        return rPage

    def getDeviceVersion(self):
        #Read registers containing chip information
        mystop=False
        nwords=2
        myaddr= [0x02 ]#0xfa
        self.setPage(0)
        self.i2c.write( self.slaveaddr, myaddr, mystop)
        #time.sleep(0.1)
        res= self.i2c.read( self.slaveaddr, nwords)
        print "  CLOCK EPROM: "
        result="\t  "
        for iaddr in res:
            result+="%#02x "%(iaddr)
        print result
        return res

    def parse_clk(self, filename):
        #Parse the configuration file produced by Clockbuilder Pro (Silicon Labs)
    	deletedcomments=""""""
    	print "\tParsing file", filename
    	with open(filename, 'rb') as configfile:
    		for i, line in enumerate(configfile):
    		    if not line.startswith('#') :
    		      if not line.startswith('Address'):
    			deletedcomments+=line
    	csvfile = StringIO.StringIO(deletedcomments)
    	cvr= csv.reader(csvfile, delimiter=',', quotechar='|')
    	#print "\tN elements  parser:", sum(1 for row in cvr)
    	#return [addr_list,data_list]
        # for item in cvr:
        #     print "rere"
        #     regAddr= int(item[0], 16)
        #     regData[0]= int(item[1], 16)
        #     print "\t  ", hex(regAddr), hex(regData[0])
        #self.configList= cvr
        regSettingList= list(cvr)
        print "\t  ", len(regSettingList), "elements"

        return regSettingList

    def writeConfiguration(self, regSettingList):
        print "\tWrite configuration:"
        #regSettingList= list(regSettingCsv)
        for counter, item in enumerate(regSettingList):
            regAddr= int(item[0], 16)
            regData=[0]
            regData[0]= int(item[1], 16)
            logging.debug("%d reg: 0x%x data: 0x%x", counter, regAddr, regData[0])
            self.writeRegister(regAddr, regData)
