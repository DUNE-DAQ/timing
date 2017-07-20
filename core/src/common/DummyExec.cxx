#include "pdt/DummyClass.hpp"

#include <iostream>
#include <cstdlib>
#include <boost/thread/thread.hpp>

#include "pdt/Logger.hpp"
#include "pdt/toolbox.hpp"
#include "pdt/SI5344Node.hpp"

#include "uhal/ConnectionManager.hpp"
#include "uhal/log/log.hpp"

enum {
	kRev1,
	kRev2
};

const std::map<uint32_t, std::string> kClockConfigMap = {
	{kRev1, "SI5344/PDTS0000.txt"},
	{kRev2, "SI5344/PDTS0003.txt"}
};

const std::map<uint32_t, uint32_t> kBoardRevisionMap = {
	{0xd880395e720b, kRev1},
	{0xd880395e501a, kRev1},
	{0xd880395e50b8, kRev1},
	{0xd880395e501b, kRev1},
	{0xd880395e7201, kRev1},
	{0xd880395e4fcc, kRev1},
	{0xd880395e5069, kRev1},
	{0xd880395e7206, kRev1},
	{0xd880395e1c86, kRev2},
	{0xd880395e2630, kRev2},
	{0xd880395e262b, kRev2},
	{0xd880395e2b38, kRev2},
	{0xd880395e1a6a, kRev2},
	{0xd880395e36ae, kRev2},
	{0xd880395e2b2e, kRev2},
	{0xd880395e2b33, kRev2},
	{0xd880395e1c81, kRev2}
};

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

   	uhal::HwInterface lOuro = lCM.getDevice("DUNE_FMC_SLAVE_TUN");
   	PDT_LOG(pdt::kNotice) << "Opening connection to uri: " << lOuro.uri();


   	PDT_LOG(pdt::kNotice) << "Soft reset";
	auto lStat = lOuro.getNode("io.csr.stat").read();
    lOuro.getNode("io.csr.ctrl.soft_rst").write(1);
    lOuro.dispatch();
	PDT_LOG(pdt::kInfo) << "io.csr.stat: " << std::showbase << std::hex << lStat;

	PDT_LOG(pdt::kNotice) << "Resetting PLL";
    lOuro.getNode("io.csr.ctrl.pll_rst").write(1);
    lOuro.dispatch();
    lOuro.getNode("io.csr.ctrl.pll_rst").write(0);
    lOuro.dispatch();

   	PDT_LOG(pdt::kNotice) << "Reading Board information";



   	PDT_LOG(pdt::kNotice) << "Reading PLL version information";
	pdt::SI5344Node lClock(lOuro.getNode("io.pll_i2c"));

   	PDT_LOG(pdt::kInfo) << "Resetting pll's i2c bus";
	lClock.reset();
	PDT_LOG(pdt::kInfo) << "SI5344 version " << std::showbase << std::hex << lClock.readDeviceVersion();


   	uhal::ValWord<uint32_t> lMasterVersion = lOuro.getNode("master.global.version").read();
   	uhal::ValWord<uint32_t> lEndpointVersion = lOuro.getNode("endpoint.version").read();
   	lOuro.dispatch();

	PDT_LOG(pdt::kInfo) << "Master version: " << std::showbase << std::hex << lMasterVersion;
	PDT_LOG(pdt::kInfo) << "Endpoint version: " << std::showbase << std::hex << lEndpointVersion;



    
	boost::this_thread::sleep_for( boost::chrono::seconds(1) );


    std::string aCfgPath = "${PDT_TESTS}/scripts/ouroboros/SI5344/PDTS0000.txt";
    aCfgPath = pdt::shellExpandPath(aCfgPath);
	PDT_LOG(pdt::kInfo) << "Loading clock configuration" << aCfgPath;
	lClock.configure(aCfgPath);

	/* code */
	return 0;
}