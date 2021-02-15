#include "pdt/EndpointDesign.hpp"
#include "pdt/FanoutDesign.hpp"
#include "pdt/PDIMasterDesign.hpp"
#include "pdt/MasterMuxDesign.hpp"

namespace dunedaq {
namespace pdt {
// In leiu of UHAL_REGISTER_DERIVED_NODE
uhal::RegistrationHelper< EndpointDesign<FMCIONode> > EndpointDesign_FMCIONode_RegistrationHelper( "EndpointDesign<FMCIONode>" );


uhal::RegistrationHelper< FanoutDesign<PC059IONode,PDIMasterNode> > FanoutDesign_PC059IONode_PDIMasterNode_RegistrationHelper( "FanoutDesign<PC059IONode,PDIMasterNode>" );

uhal::RegistrationHelper< MasterMuxDesign<PC059IONode,PDIMasterNode> > MasterMuxDesign_PC059IONode_PDIMasterNode_RegistrationHelper( "MasterMuxDesign<PC059IONode,PDIMasterNode>" );

uhal::RegistrationHelper< PDIMasterDesign<TLUIONode> > PDIMasterDesign_TLUIONode_RegistrationHelper( "PDIMasterDesign<TLUIONode>" );
uhal::RegistrationHelper< PDIMasterDesign<FMCIONode> > PDIMasterDesign_FMCIONode_RegistrationHelper( "PDIMasterDesign<FMCIONode>" );

uhal::RegistrationHelper< PDIMasterDesign<SIMIONode> > PDIMasterDesign_SIMIONode_RegistrationHelper( "PDIMasterDesign<SIMIONode>" );

} // namespace pdt
} // namespace dunedaq