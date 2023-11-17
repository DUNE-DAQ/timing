/**
 * @file definitions.hpp
 *
 * Timing definitions and types.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_DEFINITIONS_HPP_
#define TIMING_INCLUDE_TIMING_DEFINITIONS_HPP_

#include "TimingIssues.hpp"

#include "logging/Logging.hpp"

#include <cmath>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

typedef std::map<std::string, uint32_t> Snapshot; // NOLINT(build/unsigned)

enum BoardType
{
  kBoardFMC,
  kBoardSim,
  kBoardPC059,
  kBoardMicrozed,
  kBoardTLU,
  kBoardFIB,
  kBoardMIB,
  kBoardPC069,
  kBoardGIB,
  kBoardUnknown = 256
};

enum CarrierType
{
  kCarrierEnclustraA35,
  kCarrierKC705,
  kCarrierMicrozed,
  kCarrierATFC,
  kCarrierAFC,
  kCarrierNexusVideo,
  kCarrierTrenzTE0712, // include fpga part number?
  kCarrierUnknown = 256
};

enum DesignType
{
  kDesignMaster,
  kDesignOuroborosSim,
  kDesignOuroboros,
  kDesignTest,
  kDesignEndpoint,
  kDesignFanout,
  kDesignOverlord,
  kDesignEndpoBICRT,
  kDesignChronos,
  kDesignBoreas,
  kDesignKerberos,
  kDesignGaia,
  kDesignUnknown = 256
};

enum BoardRevision
{
  kFMCRev1,
  kFMCRev2,
  kFMCRev3,
  kFMCRev4,
  kPC059Rev1,
  kTLURev1,
  kSIMRev1,
  kFIBRev1,
  kMIBRev1,
  kPC069a,
  kPC069b,
  kMIBRev2,
  kGIBRev1,
  kBoardRevisionUnknown = 256
};

enum FixedLengthCommandType
{
  TimeSync,  
  Echo,      
  SpillStart, 
  SpillStop,  
  RunStart,
  RunStop,   
  WibCalib,  
  SSPCalib,   
  FakeTrig0,  
  FakeTrig1,
  FakeTrig2, 
  FakeTrig3, 
  BeamTrig,   
  NoBeamTrig, 
  ExtFakeTrig
};

struct ActiveEndpointConfig
{
  std::string id;
  uint32_t adr; // NOLINT(build/unsigned)
  int32_t fanout;
  uint32_t mux; // NOLINT(build/unsigned)
  bool active;
  uint32_t cdelay; // NOLINT(build/unsigned)
  uint32_t fdelay; // NOLINT(build/unsigned)
  uint32_t pdelay; // NOLINT(build/unsigned)

  ActiveEndpointConfig(std::string aId,
                       uint32_t aAdr, // NOLINT(build/unsigned)
                       int32_t aFanout = -1,
                       uint32_t aMux = 0,           // NOLINT(build/unsigned)
                       uint32_t coarse_delayay = 0, // NOLINT(build/unsigned)
                       uint32_t fine_delayay = 0)   // NOLINT(build/unsigned)
    : id(aId)
    , adr(aAdr)
    , fanout(aFanout)
    , mux(aMux)
    , active(true)
    , cdelay(coarse_delayay)
    , fdelay(fine_delayay)
    , pdelay(0)
  {}
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_DEFINITIONS_HPP_
