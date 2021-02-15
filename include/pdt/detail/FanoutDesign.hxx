#include <string>
#include <sstream>

namespace dunedaq::pdt
{


// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO, class MST>
uhal::Node* FanoutDesign<IO,MST>::clone() const { 
    return new FanoutDesign<IO,MST> ( static_cast<const FanoutDesign<IO,MST>&> ( *this ) );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
FanoutDesign<IO,MST>::FanoutDesign(const uhal::Node& aNode) : MasterMuxDesign<IO,MST>(aNode) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
FanoutDesign<IO,MST>::~FanoutDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
std::string FanoutDesign<IO,MST>::get_status(bool aPrint) const {
	std::stringstream lStatus;
	lStatus << this->getIONode().getPLLStatus();
	lStatus << this->getMasterNode().get_status();
	// TODO fanout specific status
	if (aPrint) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void FanoutDesign<IO,MST>::configure() const {
	// fanout mode hard-coded, to be passed in as parameter in future
	uint32_t lFanoutMode = 0;
	
	// Hard reset
	this->reset(lFanoutMode);

	if (!lFanoutMode) {
		// Set timestamp to current time
		this->getMasterNode().syncTimestamp();

		// Enable spill interface
		this->getMasterNode().enableSpillInterface();

		// Trigger interface configuration
		this->getMasterNode().resetExternalTriggersEndpoint();
		this->getMasterNode().enableExternalTriggers();
	}
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void FanoutDesign<IO,MST>::reset(uint32_t aFanoutMode, const std::string& aClockConfigFile) const {
	this->getIONode().reset(aFanoutMode, aClockConfigFile);
	// 0 - fanout mode, outgoing data comes from sfp
	// 1 - standalone mode, outgoing data comes from local master
	uhal::Node::getNode("switch.csr.ctrl.master_src").write(aFanoutMode);
	uhal::Node::getClient().dispatch();
}
//-----------------------------------------------------------------------------


////-----------------------------------------------------------------------------
//template<class IO, class MST>
//void FanoutDesign<IO,MST>::reset(const std::string& aClockConfigFile) const {
//	this->reset(0, aClockConfigFile);
//}
////-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
uint32_t FanoutDesign<IO,MST>::measureEndpointRTT(uint32_t aAddr, bool aControlSFP, uint32_t aSFPMUX) const {
	auto lFanoutMode = uhal::Node::getNode("switch.csr.ctrl.master_src").read();
	uhal::Node::getClient().dispatch();

	if (!lFanoutMode.value()) {
		std::ostringstream lMsg;
        lMsg << " Fanout unit " << uhal::Node::getId() << " is in fanout mode. Measure endpoint RTT should be called from master device.";
        throw UnsupportedFunction(ERS_HERE, uhal::Node::getId(), lMsg.str());
	}
	return MasterMuxDesign<IO,MST>::measureEndpointRTT(aAddr,aControlSFP,aSFPMUX);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO, class MST>
void FanoutDesign<IO,MST>::applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT, bool aControlSFP, uint32_t aSFPMUX) const {
	auto lFanoutMode = uhal::Node::getNode("switch.csr.ctrl.master_src").read();
	uhal::Node::getClient().dispatch();

	if (!lFanoutMode.value()) {
		std::ostringstream lMsg;
        lMsg << "Fanout unit " << uhal::Node::getId() << " is in fanout mode. Apply endpoint delay should be called from master device." ;
        throw UnsupportedFunction(ERS_HERE, uhal::Node::getId(), lMsg.str());
	}
	MasterMuxDesign<IO,MST>::applyEndpointDelay(aAddr,aCDel,aFDel,aPDel,aMeasureRTT,aControlSFP,aSFPMUX);
}
//-----------------------------------------------------------------------------

}