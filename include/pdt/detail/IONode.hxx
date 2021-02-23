namespace dunedaq::pdt {

//-----------------------------------------------------------------------------
template < class T> 
std::unique_ptr<const T>
IONode::get_i2c_device(const std::string& i2c_bus_name, const std::string& i2c_device_name) const {
	return std::unique_ptr<T>(new T(&getNode<I2CMasterNode>(i2c_bus_name) , getNode<I2CMasterNode>(i2c_bus_name).get_slave_address(i2c_device_name)));
}
//-----------------------------------------------------------------------------

} // namespace pdt