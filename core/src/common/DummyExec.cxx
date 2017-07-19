#include "pdt/DummyClass.hpp"

#include <iostream>
#include <cstdlib>
#include "pdt/Logger.hpp"
#include "pdt/toolbox.hpp"

#include "uhal/ConnectionManager.hpp"

int main(int argc, char const *argv[])
{
	namespace fs = boost::filesystem;

	const std::string kEnvVar("PDT_TESTS");
	const char* lTestDir = nullptr;
	if(lTestDir = std::getenv(kEnvVar.c_str())) {
        PDT_LOG(pdt::kNotice) << "Your PDT_TESTS is: " << lTestDir << std::endl;
   	} else {
   	    PDT_LOG(pdt::kError) << kEnvVar << " is not defined" << std::endl;
    	exit(-1);
	}

   	fs::path lConnections = fs::path(lTestDir) / "etc" / "connections.xml";

	pdt::throwIfNotFile(lConnections.string());

   	uhal::ConnectionManager lCM("file://"+lConnections.string());
	pdt::DummyClass dummy();
	/* code */
	return 0;
}