# configure i2c expander to point to pll

# create instance of pll class
lI2CBusNode = lDevice.getNode("io.i2c")
lSIChip = SI534xSlave(lI2CBusNode, lI2CBusNode.getSlave('ClkGen').getI2CAddress())

# read pll version
lSIVersion = lSIChip.readDeviceVersion()
echo("PLL version : "+style(hex(lSIVersion), fg='blue'))

# upload a pll config file
lClockConfigPath = "/path/to/config/file.txt"

lSIChip.configure(lClockConfigPath)
echo("PLL configuration id: {}".format(style(lSIChip.readConfigID(), fg='green')))