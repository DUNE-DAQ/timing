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
#define	TIMING_INCLUDE_TIMING_DEFINITIONS_HPP_

#include <map>
#include <stdint.h>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

typedef std::map<std::string, uint32_t> Snapshot;

enum BoardType {kBoardFMC, kBoardSim, kBoardPC059, kBoardMicrozed, kBoardTLU, kBoardUnknown};
enum CarrierType {kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed, kCarrierATFC, kCarrierUnknown};
enum DesignType {kDesignMaster, kDesignOuroborosSim, kDesignOuroboros, kUnused, kDesignEndpoint, kDesignFanout, kDesignOverlord, kDesignEndpoBICRT, kDesignChronos, kDesignBoreas, kDesignUnknown};
enum BoardRevision {kFMCRev1, kFMCRev2, kFMCRev3, kPC059Rev1, kTLURev1, kSIMRev1};

const uint32_t g_dune_sp_clock_in_hz = 50e6;
const uint32_t g_event_size = 6;
const uint32_t g_hsi_event_size = 5;

const std::map<BoardType, std::string> g_board_type_map = {
    {kBoardFMC, "fmc"},
    {kBoardSim, "sim"},
    {kBoardPC059, "pc059"},
    {kBoardMicrozed, "microzed"},
    {kBoardTLU, "tlu"}
};


const std::map<CarrierType, std::string> g_carrier_type_map = {
    {kCarrierEnclustraA35, "enclustra-a35"},
    {kCarrierKC705, "kc705"},
    {kCarrierMicrozed, "microzed"},
    {kCarrierATFC, "atfc"}

};

const std::map<DesignType, std::string> g_design_type_map = {
    {kDesignMaster, "master"},
    {kDesignOuroboros, "ouroboros"},
    {kDesignOuroborosSim, "ouroboros-sim"},
    {kDesignEndpoint, "endpoint"},
    {kDesignFanout, "fanout"},
    {kDesignOverlord, "overlord"},
    {kDesignEndpoBICRT, "endpoint-bi-crt"},
    {kDesignChronos, "chronos"},
    {kDesignBoreas, "boreas"},
};

const std::map<BoardRevision, std::string> g_board_revision_map = {
    {kFMCRev1, "kFMCRev1"},
    {kFMCRev2, "kFMCRev2"},
    {kFMCRev3, "kFMCRev3"},
    {kPC059Rev1, "kPC059Rev1"},
    {kTLURev1, "kTLURev1"},
    {kSIMRev1, "kSIMRev1"}
};

const std::map<uint64_t, BoardRevision> g_board_uid_revision_map = {
    {0xd880395e720b, kFMCRev1},
    {0xd880395e501a, kFMCRev1},
    {0xd880395e50b8, kFMCRev1},
    {0xd880395e501b, kFMCRev1},
    {0xd880395e7201, kFMCRev1},
    {0xd880395e4fcc, kFMCRev1},
    {0xd880395e5069, kFMCRev1},
    {0xd880395e7206, kFMCRev1},
    {0xd880395e1c86, kFMCRev2},
    {0xd880395e2630, kFMCRev2},
    {0xd880395e262b, kFMCRev2},
    {0xd880395e2b38, kFMCRev2},
    {0xd880395e1a6a, kFMCRev2},
    {0xd880395e36ae, kFMCRev2},
    {0xd880395e2b2e, kFMCRev2},
    {0xd880395e2b33, kFMCRev2},
    {0xd880395e1c81, kFMCRev2},
    {0x049162b67ce6, kFMCRev3},
    {0x049162b62947, kFMCRev3},
    {0x049162b67cdf, kFMCRev3},
    {0x49162b62050,  kFMCRev3},
    {0x49162b62951,  kFMCRev3},
    {0x49162b675e3,  kFMCRev3},
    {0xd88039d980cf, kPC059Rev1},
    {0xd88039d98adf, kPC059Rev1},
    {0xd88039d92491, kPC059Rev1},
    {0xd88039d9248e, kPC059Rev1},
    {0xd88039d98ae9, kPC059Rev1},
    {0xd88039d92498, kPC059Rev1},
    {0x5410ecbba408, kTLURev1},
    {0x5410ecbb9426, kTLURev1},
    //{0x801f12f5ce48, kFIBRev1},
    //{0x801f12f5e9ae, kFIBRev1},
    {0x49162b65025,  kFMCRev3},
    {0x49162b62948,  kFMCRev3},
    {0x49162b675ea,  kFMCRev3},
};

const std::map<std::string, std::string> g_clock_config_map = {
    {"kFMCRev1_enclustra-a35_endpoint", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt"},
    {"kFMCRev2_enclustra-a35_endpoint", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt"},
    {"kFMCRev3_enclustra-a35_endpoint", "devel/Si5394-053endptr_62-5MHz_4kHz-Registers.txt"},
    
    {"kFMCRev1_enclustra-a35_endpoint-bi-crt", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt"},
    {"kFMCRev2_enclustra-a35_endpoint-bi-crt", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt"},
    {"kFMCRev3_enclustra-a35_endpoint-bi-crt", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt"},
    
    {"kFMCRev1_enclustra-a35_ouroboros", "devel/Si5344-053master_312.5_mhz-Registers.txt"},
    {"kFMCRev2_enclustra-a35_ouroboros", "devel/Si5344-053master_312.5_mhz-Registers.txt"},
    {"kFMCRev3_enclustra-a35_ouroboros", "devel/Si5394-RevA-94mst625-Registers.txt"},

    {"kFMCRev1_enclustra-a35_boreas", "devel/Si5344-053master_312.5_mhz-Registers.txt"},
    {"kFMCRev2_enclustra-a35_boreas", "devel/Si5344-053master_312.5_mhz-Registers.txt"},
    {"kFMCRev3_enclustra-a35_boreas", "devel/Si5394-RevA-94mst625-Registers.txt"},
    
    {"kTLURev1_enclustra-a35_overlord", "devel/DUNE_TLU-DUNTLU09-Registers_62.5_mhz.txt"},
    {"kTLURev1_enclustra-a35_boreas",   "devel/DUNE_TLU-DUNTLU09-Registers_62.5_mhz.txt"},
    
    {"kPC059Rev1_enclustra-a35_fanout_mode0", "devel/pc059_sfp_in_312_mhz-059_0s62-Registers.txt"},     // fanout mode, assuming sfp is the upstream input
    {"kPC059Rev1_enclustra-a35_fanout_mode1", "devel/pc059_standalone_312_mhz-059_1_62-Registers.txt"}, // stand-alone mode

    {"kPC059Rev1_enclustra-a35_ouroboros", "devel/pc059_standalone_312_mhz-059_1_62-Registers.txt"}
};

const std::map<uint32_t, std::string> g_command_map = {
    {0x0, "TimeSync"},
    {0x1, "Echo"},
    {0x2, "SpillStart"},
    {0x3, "SpillStop"},
    {0x4, "RunStart"},
    {0x5, "RunStop"},
    {0x6, "WibCalib"},
    {0x7, "SSPCalib"},
    {0x8, "FakeTrig0"},
    {0x9, "FakeTrig1"},
    {0xa, "FakeTrig2"},
    {0xb, "FakeTrig3"},
    {0xc, "BeamTrig"},
    {0xd, "NoBeamTrig"},
    {0xe, "ExtFakeTrig"}
};

const uint32_t g_command_number = g_command_map.size();

const std::map<uint32_t, std::string> g_endpoint_state_map = {
    { 0x0, "Standing by (0x0)"                              } , // 0b0000 when W_RST, -- Starting state after reset
    { 0x1, "Waiting for SFP signal (0x1)"                   } , // 0b0001 when W_LINK, -- Waiting for SFP LOS to go low
    { 0x2, "Waiting for good frequency check (0x2)"         } , // 0b0010 when W_FREQ, -- Waiting for good frequency check
    { 0x3, "Waiting for phase adjustment to complete (0x3)" } , // 0b0011 when W_ADJUST, -- Waiting for phase adjustment to complete
    { 0x4, "Waiting for comma alignment (0x4)"              } , // 0b0100 when W_ALIGN, -- Waiting for comma alignment, stable 62.5 (50) MHz phase
    { 0x5, "Waiting for 8b10 decoder good packet (0x5)"     } , // 0b0101 when W_LOCK, -- Waiting for 8b10 decoder good packet
    { 0x6, "Waiting for phase adjustment command (0x6)"     } , // 0b0110 when W_PHASE, -- Waiting for phase adjustment command
    { 0x7, "Waiting for time stamp initialisation (0x7)"    } , // 0b0111 when W_RDY, -- Waiting for time stamp initialisation
    { 0x8, "Ready (0x8)"                                    } , // 0b1000 when RUN, -- Good to go
    { 0xc, "Error in Rx (0xc)"                              } , // 0b1100 when ERR_R, -- Error in rx
    { 0xd, "Error in time stamp check (0xd)"                } , // 0b1101 when ERR_T; -- Error in time stamp check
    { 0xe, "Error in physical layer after lock (0xe)"       } , // 0b1110 when ERR_P; -- Physical layer error after lock
};


struct ActiveEndpointConfig {
    std::string id;
    uint32_t adr;
    int32_t fanout;
    uint32_t mux;
    bool active;
    uint32_t cdelay;
    uint32_t fdelay;
    uint32_t pdelay;

    ActiveEndpointConfig(std::string aId, uint32_t aAdr, int32_t aFanout=-1, uint32_t aMux=0, uint32_t coarse_delayay=0, uint32_t fine_delayay=0) : 
    id(aId), 
    adr(aAdr), 
    fanout(aFanout),
    mux(aMux), 
    active(true),
    cdelay(coarse_delayay),
    fdelay(fine_delayay),
    pdelay(0)
    {

    }
};

struct EndpointRTTResult {
    std::string id;
    uint32_t adr;
    int32_t fanout;
    uint32_t mux;
    int32_t measuredRTT;

    EndpointRTTResult(ActiveEndpointConfig ept_config, uint32_t aMeasuredRTT) :
    id(ept_config.id),
    adr(ept_config.adr),
    fanout (ept_config.fanout),
    mux(ept_config.mux),
    measuredRTT(aMeasuredRTT) {
    }
};

} // namespace timing
} // namespace dunedaq

#endif	// TIMING_INCLUDE_TIMING_DEFINITIONS_HPP_