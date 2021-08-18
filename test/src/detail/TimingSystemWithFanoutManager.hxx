namespace dunedaq::timing {

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::TimingSystemWithFanoutManager(std::string cf)
  : TimingSystemManager<MST_TOP, EPT_TOP>(cf)
  , fanoutHardwareNames({ "PROD_FANOUT_0" })
{
  // set up fanout devices
  for (auto it = fanoutHardwareNames.begin(); it != fanoutHardwareNames.end(); ++it) {
    fanoutHardware.push_back(this->connectionManager->getDevice(*it));
  }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::~TimingSystemWithFanoutManager()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
uint32_t
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::getNumberOfFanouts() const
{
  return fanoutHardware.size();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
const FAN_TOP&
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::getFanout(uint32_t aFanoutId) const
{
  return fanoutHardware.at(aFanoutId).getNode<FAN_TOP>("");
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::printSystemStatus() const
{
  std::cout << "Timing master status" << std::endl;
  for (uint32_t i = 0; i < this->getNumberOfMasters(); ++i)
    this->getMaster(i).get_status(true);

  std::cout << "Timing fanout status" << std::endl;
  for (uint32_t i = 0; i < getNumberOfFanouts(); ++i)
    getFanout(i).get_status(true);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::resetSystem() const
{
  for (uint32_t i = 0; i < this->getNumberOfMasters(); ++i)
    this->getMaster(i).reset();
  for (uint32_t i = 0; i < getNumberOfFanouts(); ++i)
    getFanout(i).reset();
  for (uint32_t i = 0; i < this->getNumberOfEndpoints(); ++i)
    this->getEndpoint(i).reset();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::configureSystem() const
{
  for (uint32_t i = 0; i < this->getNumberOfMasters(); ++i)
    this->getMaster(i).configure();
  for (uint32_t i = 0; i < getNumberOfFanouts(); ++i)
    getFanout(i).configure();
  // apply endpoint delays
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::resetPartition(uint32_t partition_id) const
{
  this->getMaster(0).get_master_node().get_partition_node(partition_id).reset();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::configurePartition(uint32_t partition_id,
                                                                             uint32_t trigger_mask,
                                                                             bool enableSpillGate) const
{
  this->getMaster(0).get_master_node().get_partition_node(partition_id).configure(trigger_mask, enableSpillGate);
  this->getMaster(0).get_master_node().get_partition_node(partition_id).enable();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::startPartition(uint32_t partition_id) const
{
  this->getMaster(0).get_master_node().get_partition_node(partition_id).start();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
void
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::stopPartition(uint32_t partition_id) const
{
  this->getMaster(0).get_master_node().get_partition_node(partition_id).stop();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
uint64_t
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::read_master_timestamp() const
{
  return this->getMaster(0).read_master_timestamp();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
uint64_t
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::measure_endpoint_rtt(uint32_t address,
                                                                               int32_t aFanout,
                                                                               uint32_t aMux) const
{

  uint32_t lRTT;

  this->getMaster(0).get_master_node().switch_endpoint_sfp(0x0, false);
  this->getMaster(0).get_master_node().switch_endpoint_sfp(address, true);

  // TODO check which fanout is active, and switch if necessary

  // set fanout rtt mux channel, and wait for fanout rtt ept to be in a good state
  if (aFanout >= 0)
    getFanout(aFanout).switch_sfp_mux_channel(aMux, true);

  // gets master rtt ept in a good state, and sends echo command (due to second argument endpoint sfp is not controlled
  // in this call, already done above)
  lRTT = this->getMaster(0).get_master_node().measure_endpoint_rtt(address, false);

  this->getMaster(0).get_master_node().switch_endpoint_sfp(address, false);

  return lRTT;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
uint64_t
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::measure_endpoint_rtt(uint32_t address) const
{
  return this->measure_endpoint_rtt(address, -1, 0);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class MST_TOP, class EPT_TOP, class FAN_TOP>
uint64_t
TimingSystemWithFanoutManager<MST_TOP, EPT_TOP, FAN_TOP>::measure_endpoint_rtt(
  const ActiveEndpointConfig& ept_config) const
{
  uint32_t lEptAdr = ept_config.adr;
  uint32_t lEptFanout = ept_config.fanout;
  uint32_t lEptMux = ept_config.mux;

  return this->measure_endpoint_rtt(lEptAdr, lEptFanout, lEptMux);
}
//-----------------------------------------------------------------------------

}