#include "pdt/DummyClass.hpp"

#include <iostream>
#include <cstdlib>
#include <boost/thread/thread.hpp>

#include "pdt/Logger.hpp"
#include "pdt/toolbox.hpp"
#include "pdt/SI534xNode.hpp"
#include "pdt/I2CMasterNode.hpp"

#include "uhal/ConnectionManager.hpp"
#include "uhal/log/log.hpp"


// ----------------------------------------------------------
int main(int argc, char const *argv[])
{
	namespace fs = boost::filesystem;

	const std::string kEnvVar("PDT_TESTS");
	const char* lTestDir = nullptr;
	if( (lTestDir = std::getenv(kEnvVar.c_str())) ) {
        PDT_LOG(pdt::kNotice) << "Your PDT_TESTS is: " << lTestDir;
   	} else {
   	    PDT_LOG(pdt::kError) << kEnvVar << " is not defined";
    	exit(-1);
	}

   	fs::path lConnections = fs::path(lTestDir) / "etc" / "connections.xml";

	pdt::throwIfNotFile(lConnections.string());

	uhal::setLogLevelTo(uhal::WarningLevel());
   	uhal::ConnectionManager lCM("file://"+lConnections.string());

   	uhal::HwInterface lOuro = lCM.getDevice("SECONDARY_TUN");
   	// uhal::HwInterface lOuro = lCM.getDevice("DUNE_FMC_MASTER_TUN");
   	PDT_LOG(pdt::kNotice) << "Opening connection to uri: " << lOuro.uri();

    lOuro.getNode().read();
    lOuro.dispatch();

    const auto& io = lOuro.getNode<pdt::I2CMasterNode>("io.uid_i2c");

    for ( const auto& n : io.getSlaves()) {
        std::cout << n << std::endl;
        io.getSlave(n);
    }

	/* code */
	return 0;
}