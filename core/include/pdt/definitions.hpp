/* 
 * File:   definitions.hpp
 * Author: ale
 *
 * Created on November 16, 2014, 7:50 PM
 */

#ifndef __PDT_DEFINITIONS_HPP__
#define	__PDT_DEFINITIONS_HPP__

#include <map>
#include <stdint.h>
#include <string>
#include <vector>

#include "pdt/Logger.hpp"

namespace pdt {

typedef std::map<std::string, uint32_t> Snapshot;

const uint32_t kSPSClockInHz = 50e6;
const uint32_t kEventSize = 6;

enum BoardType {kBoardFMC, kBoardSim, kBoardPC059, kBoardMicrozed, kBoardTLU, kBoardFIB, kBoardUnknown};
enum CarrierType {kCarrierEnclustraA35, kCarrierKC705, kCarrierMicrozed, kCarrierATFC, kCarrierAFC, kCarrierUnknown};
enum DesignType {kDesingMaster, kDesignOuroborosSim, kDesignOuroboros, kUnused, kDesignEndpoint, kDesingFanout, kDesingOverlord, kDesingEndpoBICRT, kDesignUnknown};
enum BoardRevision {kFMCRev1, kFMCRev2, kFMCRev3, kPC059Rev1, kTLURev1, kSIMRev1, kFIBRev1};

const std::map<BoardType, std::string> kBoardTypeMap = {
    {kBoardFMC, "fmc"},
    {kBoardSim, "sim"},
    {kBoardPC059, "pc059"},
    {kBoardMicrozed, "microzed"},
    {kBoardTLU, "tlu"},
    {kBoardFIB, "fib"}
};


const std::map<CarrierType, std::string> kCarrierTypeMap = {
    {kCarrierEnclustraA35, "enclustra-a35"},
    {kCarrierKC705, "kc705"},
    {kCarrierMicrozed, "microzed"},
    {kCarrierATFC, "atfc"},
    {kCarrierAFC, "afc"}

};

const std::map<DesignType, std::string> kDesignTypeMap = {
    {kDesingMaster, "master"},
    {kDesignOuroboros, "ouroboros"},
    {kDesignOuroborosSim, "ouroboros-sim"},
    {kDesignEndpoint, "endpoint"},
    {kDesingFanout, "fanout"},
    {kDesingOverlord, "overlord"},
    {kDesingEndpoBICRT, "endpoint-bi-crt"}
};

const std::map<BoardRevision, std::string> kBoardRevisionMap = {
    {kFMCRev1, "kFMCRev1"},
    {kFMCRev2, "kFMCRev2"},
    {kFMCRev3, "kFMCRev3"},
    {kPC059Rev1, "kPC059Rev1"},
    {kTLURev1, "kTLURev1"},
    {kSIMRev1, "kSIMRev1"},
    {kFIBRev1, "kFIBRev1"}
};

const std::map<uint64_t, BoardRevision> kBoardUIDRevisionMap = {
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
    {0x801f12f5ce48, kFIBRev1},
    {0x801f12f5e9ae, kFIBRev1},
    {0x49162b65025,  kFMCRev3}

};

const std::map<std::string, std::string> kClockConfigMap = {
    {"kFMCRev1_enclustra-a35_endpoint", "devel/ENDPOINT-Si5344-50MHzRef.txt"},
    {"kFMCRev2_enclustra-a35_endpoint", "devel/ENDPOINT-Si5344-50MHzRef.txt"},
    {"kFMCRev3_enclustra-a35_endpoint", "devel/ENDPOINT-Si5344-50MHzRef.txt"},
    {"kFIBRev1_enclustra-a35_endpoint", "devel/ENDPOINT-Si5344-50MHzRef.txt"},

    {"kFMCRev1_enclustra-a35_endpoint-bi-crt", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt"},
    {"kFMCRev2_enclustra-a35_endpoint-bi-crt", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt"},
    {"kFMCRev3_enclustra-a35_endpoint-bi-crt", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt"},
    
    {"kFMCRev1_enclustra-a35_ouroboros", "SI5344/PDTS0000.txt"},
    {"kFMCRev2_enclustra-a35_ouroboros", "SI5344/PDTS0003.txt"},
    {"kFMCRev3_enclustra-a35_ouroboros", "SI5344/PDTS0003.txt"},
    {"kPC059Rev1_enclustra-a35_ouroboros", "SI5345/PDTS0005.txt"},

    {"kFIBRev1_enclustra-a35_ouroboros", "SI5345/PDTS0005.txt"},

    {"kTLURev1_enclustra-a35_overlord", "wr/TLU_EXTCLK_10MHZ_NOZDM.txt"},
    
    {"kPC059Rev1_enclustra-a35_fanout_mode0", "wr/FANOUT_PLL_WIDEBW_SFPIN.txt"}, // fanout mode
    {"kPC059Rev1_enclustra-a35_fanout_mode1", "devel/PDTS_PC059_FANOUT.txt"},    // stand-alone mode

    {"kFIBRev1_enclustra-a35_fanout_mode1", "devel/PDTS_PC059_FANOUT.txt"},    // stand-alone mode
    {"kFIBRev1_afc_fanout_mode1", "SI5395/PDTS0004.txt"},    // stand-alone mode
    

};

const std::map<uint32_t, std::string> kCommandMap = {
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

const uint32_t kCommandNumber = kCommandMap.size();

const std::map<uint32_t, std::string> kEndpointStateMap = {
    { 0x0, "Standing by (0x0)"                          } , // 0b0000 when W_RST, -- Starting state after reset
    { 0x1, "Waiting SFP for signal (0x1)"               } , // 0b0001 when W_SFP, -- Waiting for SFP LOS to go low
    { 0x2, "Waiting CDR lock (0x2)"                     } , // 0b0010 when W_CDR, -- Waiting for CDR lock
    { 0x3, "Waiting for good frequency check (0x3)"     } , // 0b0011 when W_FREQ, -- Waiting for good frequency check
    { 0x4, "Waiting for comma alignment (0x4)"          } , // 0b0100 when W_ALIGN, -- Waiting for comma alignment           , stable 50MHz phase
    { 0x5, "Waiting for 8b10 decoder good packet (0x5)" } , // 0b0101 when W_LOCK, -- Waiting for 8b10 decoder good packet
    { 0x6, "Waiting for phase adjustment command (0x6)" } , // 0b0110 when W_PHASE, -- Waiting for phase adjustment command
    { 0x7, "Waiting for time stamp initialisation (0x7)"} , // 0b0111 when W_RDY, -- Waiting for time stamp initialisation
    { 0x8, "Ready (0x8)"                                } , // 0b1000 when RUN, -- Good to go
    { 0xc, "Error in Rx (0xc)"                          } , // 0b1100 when ERR_R, -- Error in rx
    { 0xd, "Error in time stamp check (0xd)"            } , // 0b1101 when ERR_T; -- Error in time stamp check
    { 0xe, "Error in physical layer after lock (0xe)"   } , // 0b1110 when ERR_P; -- Physical layer error after lock
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

    ActiveEndpointConfig(std::string aId, uint32_t aAdr, int32_t aFanout=-1, uint32_t aMux=0, uint32_t aCDelay=0, uint32_t aFDelay=0) : 
    id(aId), 
    adr(aAdr), 
    fanout(aFanout),
    mux(aMux), 
    active(true),
    cdelay(aCDelay),
    fdelay(aFDelay),
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

    EndpointRTTResult(ActiveEndpointConfig aEptConfig, uint32_t aMeasuredRTT) :
    id(aEptConfig.id),
    adr(aEptConfig.adr),
    fanout (aEptConfig.fanout),
    mux(aEptConfig.mux),
    measuredRTT(aMeasuredRTT) {
    }
};

} // namespace pdt

#endif	/* __PDT_DEFINITIONS_HPP__ */

