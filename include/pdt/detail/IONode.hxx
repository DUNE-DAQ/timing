namespace dunedaq::pdt {

//-----------------------------------------------------------------------------
template < class T> 
std::unique_ptr<const T>
IONode::get_i2c_device(const std::string& i2cBusName, const std::string& i2cDeviceName) const {
	return std::unique_ptr<T>(new T(&getNode<I2CMasterNode>(i2cBusName) , getNode<I2CMasterNode>(i2cBusName).get_slave_address(i2cDeviceName)));
}
//-----------------------------------------------------------------------------

} // namespace pdt