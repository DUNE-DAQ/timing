/**
 * @file test_setupboard.cxx
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "logging/Logging.hpp"

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
#include <map>
#include <string>
#include <vector>

enum
{
  kRev1 = 1,
  kRev2 = 2
};

const std::map<uint32_t, std::string> kClockConfigMap = { { kRev1, "SI5344/PDTS0000.txt" }, // NOLINT(build/unsigned)
                                                          { kRev2, "SI5344/PDTS0003.txt" } };

const std::map<uint32_t, uint32_t> kBoardRevisionMap = { // NOLINT(build/unsigned)
  { 0xd880395e720b, kRev1 }, { 0xd880395e501a, kRev1 }, { 0xd880395e50b8, kRev1 }, { 0xd880395e501b, kRev1 },
  { 0xd880395e7201, kRev1 }, { 0xd880395e4fcc, kRev1 }, { 0xd880395e5069, kRev1 }, { 0xd880395e7206, kRev1 },
  { 0xd880395e1c86, kRev2 }, { 0xd880395e2630, kRev2 }, { 0xd880395e262b, kRev2 }, { 0xd880395e2b38, kRev2 },
  { 0xd880395e1a6a, kRev2 }, { 0xd880395e36ae, kRev2 }, { 0xd880395e2b2e, kRev2 }, { 0xd880395e2b33, kRev2 },
  { 0xd880395e1c81, kRev2 }
};

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

  uhal::HwInterface lOuro = lCM.getDevice("DUNE_FMC_SLAVE_TUN");
  // uhal::HwInterface lOuro = lCM.getDevice("DUNE_FMC_MASTER_TUN");
  PDT_LOG(pdt::kNotice) << "Opening connection to uri: " << lOuro.uri();

  //-----------------------------------------------------
  // Soft reset
  PDT_LOG(pdt::kNotice) << "Soft reset";
  auto lStat = lOuro.getNode("io.csr.stat").read();
  lOuro.getNode("io.csr.ctrl.soft_rst").write(1);
  lOuro.dispatch();
  PDT_LOG(pdt::kInfo) << "io.csr.stat: " << std::showbase << std::hex << lStat;

  // 1 sec nap
  boost::this_thread::sleep_for(boost::chrono::seconds(1));

  //-----------------------------------------------------
  // Pll reset
  PDT_LOG(pdt::kNotice) << "Resetting PLL";
  lOuro.getNode("io.csr.ctrl.pll_rst").write(1);
  lOuro.dispatch();
  lOuro.getNode("io.csr.ctrl.pll_rst").write(0);
  lOuro.dispatch();

  //-----------------------------------------------------
  // Clocking setup
  const pdt::I2CMasterNode& lUID = lOuro.getNode<pdt::I2CMasterNode>("io.uid_i2c");
  TLOG() << "uid clock i2c clock prescale " << std::showbase << std::hex
         << (uint32_t)lUID.getI2CClockPrescale(); // NOLINT(build/unsigned)

  TLOG() << "A: " << std::hex << (uint32_t)lUID.getSlaveAddress("A"); // NOLINT(build/unsigned)
  TLOG() << "B: " << std::hex << (uint32_t)lUID.getSlaveAddress("B"); // NOLINT(build/unsigned)
  lUID.getSlave("A").writeI2C(0x01, 0x7f);
  uint32_t x = lUID.getSlave("A").readI2C(0x01); // NOLINT(build/unsigned)
  PDT_LOG(pdt::kInfo) << "I2c enable lines: " << x;

  // return -1;
  // uid_I2C.write(0x21, [0x01, 0x7f], True)
  // uid_I2C.write(0x21, [0x01], False)
  // res = uid_I2C.read(0x21, 1)

  std::vector<uint8_t> lVals = lUID.getSlave("B").readI2CArray(0xfa, 6); // NOLINT(build/unsigned)
  uint64_t lUniqueId(0x0);                                               // NOLINT(build/unsigned)
  for (uint8_t lVal : lVals) {                                           // NOLINT(build/unsigned)
    // std::cout << (uint64_t)lVal << std::endl;
    lUniqueId = (lUniqueId << 8) | lVal;
  }

  PDT_LOG(pdt::kInfo) << "Unique ID PROM " << std::showbase << std::hex << lUniqueId;

  uint32_t lRevision; // NOLINT(build/unsigned)
  try {
    lRevision = kBoardRevisionMap.at(lUniqueId);
  } catch (...) { // TODO: check what exception map::at throws
    PDT_LOG(pdt::kError) << "No revision associated to UID " << std::showbase << std::hex << lUniqueId;
    exit(-1);
  }

  // uid_I2C.write(0x53, [0xfa], False)
  // res = uid_I2C.read(0x53, 6)
  // id = 0
  // for i in res:
  // 	id = (id << 8) | int(i)
  // print "Unique ID PROM / board rev:", hex(id), brd_rev[id]

  // return -1;

  //-----------------------------------------------------
  PDT_LOG(pdt::kNotice) << "Reading Board information";

  //-----------------------------------------------------
  PDT_LOG(pdt::kNotice) << "Reading PLL version information";
  const pdt::SI534xNode& lClock = lOuro.getNode<pdt::SI534xNode>("io.pll_i2c");
  TLOG() << "si5344 clock i2c clock prescale " << std::showbase << std::hex
         << (uint32_t)lClock.getI2CClockPrescale(); // NOLINT(build/unsigned)

  // PDT_LOG(pdt::kInfo) << "Resetting pll's i2c bus";
  lClock.reset();
  PDT_LOG(pdt::kInfo) << "SI5344 version " << std::showbase << std::hex << lClock.readDeviceVersion();

  //-----------------------------------------------------
  uhal::ValWord<uint32_t> lMasterVersion = lOuro.getNode("master.global.version").read(); // NOLINT(build/unsigned)
  uhal::ValWord<uint32_t> lEndpointVersion = lOuro.getNode("endpoint.version").read();    // NOLINT(build/unsigned)
  lOuro.dispatch();

  PDT_LOG(pdt::kInfo) << "Master version: " << std::showbase << std::hex << lMasterVersion;
  PDT_LOG(pdt::kInfo) << "Endpoint version: " << std::showbase << std::hex << lEndpointVersion;
  //-----------------------------------------------------

  boost::this_thread::sleep_for(boost::chrono::seconds(1));

  pdt::Log::setLogThreshold(pdt::kDebug1);
  pdt::Log::setLogThreshold(pdt::kWarning);

  //------------------------------------------------------
  std::string lClockConfig;
  try {
    lClockConfig = kClockConfigMap.at(lRevision);
  } catch (...) {
    PDT_LOG(pdt::kError) << "Board revisions " << lRevision << " has no associated clock configuration";
    exit(-1);
  }

  std::string aCfgPath = "${TIMING_SHARE}/scripts/ouroboros/" + lClockConfig;

  aCfgPath = pdt::shellExpandPath(aCfgPath);
  PDT_LOG(pdt::kInfo) << "Loading clock configuration" << aCfgPath;
  lClock.configure(aCfgPath);
  pdt::Log::setLogThreshold(pdt::kInfo);

  //------------------------------------------------------
  for (uint32_t i(0); i < 2; ++i) { // NOLINT(build/unsigned)
    lOuro.getNode("io.freq.ctrl.chan_sel").write(i);
    lOuro.getNode("io.freq.ctrl.en_crap_mode").write(0);
    lOuro.dispatch();
    boost::this_thread::sleep_for(boost::chrono::seconds(1));
    auto fq = lOuro.getNode("io.freq.freq.count").read();
    auto fv = lOuro.getNode("io.freq.freq.valid").read();
    lOuro.dispatch();
    PDT_LOG(pdt::kInfo) << "Freq: " << i << " " << fv << " " << (fq * 119.20928 / 1000000);
  }

  // What does this do?
  lOuro.getNode("io.csr.ctrl.sfp_tx_dis").write(0);
  lOuro.dispatch();

  lOuro.getNode("io.csr.ctrl.rst").write(1);
  lOuro.dispatch();
  lOuro.getNode("io.csr.ctrl.rst").write(0);
  lOuro.dispatch();

  return 0;
} // NOLINT(readability/fn_size)