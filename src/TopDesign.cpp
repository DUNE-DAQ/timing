#include "timing/EndpointDesign.hpp"
#include "timing/FanoutDesign.hpp"
#include "timing/OverlordDesign.hpp"
#include "timing/OuroborosDesign.hpp"
#include "timing/OuroborosMuxDesign.hpp"
#include "timing/MasterMuxDesign.hpp"
#include "timing/BoreasDesign.hpp"

namespace dunedaq {
namespace timing {
// In leiu of UHAL_REGISTER_DERIVED_NODE

// Endpoint
uhal::RegistrationHelper< EndpointDesign<FMCIONode> > EndpointDesign_FMCIONode_RegistrationHelper( "EndpointDesign<FMCIONode>" );


// Fanout
uhal::RegistrationHelper< FanoutDesign<PC059IONode,PDIMasterNode> > FanoutDesign_PC059IONode_PDIMasterNode_RegistrationHelper( "FanoutDesign<PC059IONode,PDIMasterNode>" );


// Ouroboros (master with endpoint)
uhal::RegistrationHelper< OuroborosDesign<SIMIONode> > OuroborosDesign_SIMIONode_RegistrationHelper( "OuroborosDesign<SIMIONode>" );
uhal::RegistrationHelper< OuroborosDesign<FMCIONode> > OuroborosDesign_FMCIONode_RegistrationHelper( "OuroborosDesign<FMCIONode>" );
uhal::RegistrationHelper< OuroborosMuxDesign<PC059IONode> > OuroborosMuxDesign_PC059IONode_RegistrationHelper( "OuroborosMuxDesign<PC059IONode>" );


// Overlord (master with trigger message receiver)
uhal::RegistrationHelper< OverlordDesign<TLUIONode> > OverlordDesign_TLUIONode_RegistrationHelper( "OverlordDesign<TLUIONode>" );
uhal::RegistrationHelper< OverlordDesign<FMCIONode> > OverlordDesign_FMCIONode_RegistrationHelper( "OverlordDesign<FMCIONode>" );

// Boreas (master with HSI)
uhal::RegistrationHelper< BoreasDesign<FMCIONode> > BoreasDesign_FMCIONode_RegistrationHelper( "BoreasDesign<FMCIONode>" );
uhal::RegistrationHelper< BoreasDesign<TLUIONode> > BoreasDesign_TLUIONode_RegistrationHelper( "BoreasDesign<TLUIONode>" );
uhal::RegistrationHelper< BoreasDesign<SIMIONode> > BoreasDesign_SIMIONode_RegistrationHelper( "BoreasDesign<SIMIONode>" );


} // namespace timing
} // namespace dunedaq