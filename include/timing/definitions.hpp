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

const std::vector<BoardType> g_library_supported_boards = {
  kBoardFMC, kBoardPC059, kBoardTLU, kBoardSim, kBoardFIB, kBoardMIB, kBoardPC069
};

const uint32_t g_event_size = 6;             // NOLINT(build/unsigned)
const uint32_t g_hsi_event_size = 5;         // NOLINT(build/unsigned)

const std::map<DesignType, std::string> g_design_type_map = {
  { kDesignMaster, "master" },
  { kDesignOuroboros, "ouroboros" },
  { kDesignOuroborosSim, "ouroboros-sim" },
  { kDesignTest, "test-design" },
  { kDesignEndpoint, "endpoint" },
  { kDesignFanout, "fanout" },
  { kDesignOverlord, "overlord" },
  { kDesignEndpoBICRT, "endpoint-bi-crt" },
  { kDesignChronos, "chronos" },
  { kDesignBoreas, "boreas" },
  { kDesignUnknown, "unknown" },
};

const std::vector<DesignType> g_library_supported_designs = {
    kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignOverlord, kDesignEndpoBICRT, kDesignChronos, kDesignBoreas
};

// NOLINTNEXTLINE(build/unsigned)
const std::map<FixedLengthCommandType, std::string> g_command_map = {
  { TimeSync, "TimeSync" }, { Echo, "Echo" }, { SpillStart, "SpillStart" }, { SpillStop, "SpillStop" }, { RunStart, "RunStart" },
  { RunStop, "RunStop" }, { WibCalib, "WibCalib" }, { SSPCalib, "SSPCalib" }, { FakeTrig0, "FakeTrig0" }, { FakeTrig1, "FakeTrig1" },
  { FakeTrig2, "FakeTrig2" }, { FakeTrig3, "FakeTrig3" }, { BeamTrig, "BeamTrig" }, { NoBeamTrig, "NoBeamTrig" }, { ExtFakeTrig, "ExtFakeTrig" }
};

const uint32_t g_command_number = g_command_map.size(); // NOLINT(build/unsigned)

struct FakeTriggerConfig
{
  double requested_rate;
  uint32_t divisor;  // NOLINT(build/unsigned)
  uint32_t prescale; // NOLINT(build/unsigned)
  double actual_rate;

  explicit FakeTriggerConfig(double rate, uint32_t clock_frequency_hz) // NOLINT
    : requested_rate(rate)
  {
    // Rate =  (clock_frequency_hz / 2^(d+8)) / p where n in [0,15] and p in [1,256]

    // DIVIDER (int): Frequency divider.

    // The division from clock_frequency_hz to the desired rate is done in three steps:
    // a) A pre-division by 256
    // b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
    // c) 1-in-n prescaling set by n = rate_div_p

    double div = ceil(log(clock_frequency_hz / (requested_rate * 256 * 256)) / log(2));
    if (div < 0) {
      divisor = 0;
    } else if (div > 15) {
      divisor = 15;
    } else {
      divisor = div;
    }

    uint32_t ps = (uint32_t)((clock_frequency_hz / (rate * 256 * (1 << divisor))) + 0.5); // NOLINT(build/unsigned)
    if (ps == 0 || ps > 256) {
      throw BadRequestedFakeTriggerRate(ERS_HERE, rate, ps);
    } else {
      prescale = ps;
    }
    actual_rate = static_cast<double>(clock_frequency_hz) / (256 * prescale * (1 << divisor));
  }

  void print() const
  {
    TLOG() << "Requested rate, actual rate: " << requested_rate << ", " << actual_rate;
    TLOG() << "prescale, divisor: " << prescale << ", " << divisor;
  }
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
