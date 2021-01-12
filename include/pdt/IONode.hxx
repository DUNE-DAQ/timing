namespace pdt {

//-----------------------------------------------------------------------------
template < class T> 
std::unique_ptr<const T>
IONode::getI2CDevice(const std::string& i2cBusName, const std::string& i2cDeviceName) const {
	return std::unique_ptr<T>(new T(&getNode<I2CMasterNode>(i2cBusName) , getNode<I2CMasterNode>(i2cBusName).getSlaveAddress(i2cDeviceName)));
}
//-----------------------------------------------------------------------------

} // namespace pdt