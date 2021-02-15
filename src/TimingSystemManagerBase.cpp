#include "pdt/TimingSystemManagerBase.hpp"

namespace dunedaq {
namespace pdt {


//-----------------------------------------------------------------------------
TimingSystemManagerBase::TimingSystemManagerBase(std::string cf) : 
	// eventually will be provided via parameters from a config file
	connectionsFile(cf),
	masterHardwareNames({"PROD_MASTER"}),
	endpointHardwareNames({"EPT_2"}),
	mMaxMeasuredRTT(0)
{
	//mExpectedEndpoints.emplace("EPT_1", ActiveEndpointConfig("EPT_1", 1, -1, 0, 0x0));
	mExpectedEndpoints.emplace("EPT_2", ActiveEndpointConfig("EPT_2", 2,  0, 0, 0x0));
	//mExpectedEndpoints.emplace("EPT_3"] = ActiveEndpointConfig("EPT_3", 3, 0, 1));
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
TimingSystemManagerBase::~TimingSystemManagerBase() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
TimingSystemManagerBase::getNumberOfMasters() const {
	return masterHardware.size();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
TimingSystemManagerBase::getNumberOfEndpoints() const {
	return endpointHardware.size();
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq