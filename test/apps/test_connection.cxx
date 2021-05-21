/**
 * @file test_connection.cxx
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "pdt/DummyClass.hpp"

#include "pdt/I2CMasterNode.hpp"
#include "pdt/Logger.hpp"
#include "pdt/SI534xNode.hpp"
#include "pdt/toolbox.hpp"

#include "uhal/ConnectionManager.hpp"
#include "uhal/log/log.hpp"

#include <boost/thread/thread.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

// ----------------------------------------------------------
int
main(int argc, char const* argv[])
{
  namespace fs = boost::filesystem;

  const std::string kEnvVar("TIMING_SHARE");
  const char* lTestDir = nullptr;
  if ((lTestDir = std::getenv(kEnvVar.c_str()))) {
    PDT_LOG(pdt::kNotice) << "Your TIMING_SHARE is: " << lTestDir;
  } else {
    PDT_LOG(pdt::kError) << kEnvVar << " is not defined";
    exit(-1);
  }

  fs::path lConnections = fs::path(lTestDir) / "config" / "etc" / "connections.xml";

  pdt::throwIfNotFile(lConnections.string());

  uhal::setLogLevelTo(uhal::WarningLevel());
  uhal::ConnectionManager lCM("file://" + lConnections.string());

  uhal::HwInterface lOuro = lCM.getDevice("SECONDARY_TUN");
  // uhal::HwInterface lOuro = lCM.getDevice("DUNE_FMC_MASTER_TUN");
  PDT_LOG(pdt::kNotice) << "Opening connection to uri: " << lOuro.uri();

  lOuro.getNode().read();
  lOuro.dispatch();

  const auto& io = lOuro.getNode<pdt::I2CMasterNode>("io.uid_i2c");

  for (const auto& n : io.getSlaves()) {
    std::cout << n << std::endl; // NOLINT
    io.getSlave(n);
  }

  return 0;
}