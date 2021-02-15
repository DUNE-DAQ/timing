#include <string>
#include <sstream>

namespace dunedaq::pdt
{

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node* PDIMasterDesign<IO>::clone() const {
    return new PDIMasterDesign<IO> ( static_cast<const PDIMasterDesign<IO>&> ( *this ) );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
PDIMasterDesign<IO>::PDIMasterDesign(const uhal::Node& aNode) : MasterDesign<IO,PDIMasterNode>(aNode) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
PDIMasterDesign<IO>::~PDIMasterDesign() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
std::string PDIMasterDesign<IO>::get_status(bool aPrint) const {
	std::stringstream lStatus;
	lStatus << this->getIONode().getPLLStatus();
	lStatus << this->getMasterNode().get_status();
	if (aPrint) std::cout << lStatus.str();
	return lStatus.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<class IO>
void PDIMasterDesign<IO>::configure() const {
	
	// Hard resets
	this->reset();

	// Set timestamp to current time
	this->getMasterNode().syncTimestamp();

	// Enable spill interface
	this->getMasterNode().enableSpillInterface();

	// Trigger interface configuration
	this->getMasterNode().resetExternalTriggersEndpoint();
	this->getMasterNode().enableExternalTriggers();
}
//-----------------------------------------------------------------------------

}