/**
 * @file definitions.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/definitions.hpp"
#include "timing/EndpointNode.hpp"
#include "timing/MasterNode.hpp"
#include "timing/IONode.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <utility>
#include <map>
#include <string>

namespace py = pybind11;

namespace dunedaq {
namespace timing {
namespace python {

std::map<std::string, FixedLengthCommandType> swap_commands_map(const std::map<FixedLengthCommandType, std::string>& command_map)
{
	std::map<std::string, FixedLengthCommandType> swapped_map;
	for (auto& cmd: command_map) {
		swapped_map.emplace( std::pair<std::string,FixedLengthCommandType>(cmd.second, cmd.first) );
	}
	return swapped_map;
}

void
register_definitions(py::module& m)
{
	py::enum_<BoardType>(m, "BoardType")
        .value("kBoardFMC", kBoardFMC)
        .value("kBoardSim", kBoardSim)
        .value("kBoardPC059", kBoardPC059)
        .value("kBoardMicrozed", kBoardMicrozed)
        .value("kBoardTLU", kBoardTLU)
        .value("kBoardFIB", kBoardFIB)
        .value("kBoardMIB", kBoardMIB)
        .value("kBoardPC069", kBoardPC069)
        .value("kBoardGIB", kBoardGIB)
        .value("kBoardUnknown", kBoardUnknown)
        .export_values();

    py::enum_<BoardRevision>(m, "BoardRevision")
        .value("kFMCRev1", kFMCRev1)
        .value("kFMCRev2", kFMCRev2)
        .value("kFMCRev3", kFMCRev3)
        .value("kFMCRev4", kFMCRev4)
        .value("kPC059Rev1", kPC059Rev1)
        .value("kTLURev1", kTLURev1)
        .value("kSIMRev1", kSIMRev1)
        .value("kFIBRev1", kFIBRev1)
        .value("kMIBRev1", kMIBRev1)
        .value("kPC069a", kPC069a)
        .value("kMIBRev2", kMIBRev2)
        .value("kGIBRev1", kGIBRev1)
        .export_values();

    py::enum_<CarrierType>(m, "CarrierType")
        .value("kCarrierEnclustraA35", kCarrierEnclustraA35)
        .value("kCarrierKC705", kCarrierKC705)
        .value("kCarrierMicrozed", kCarrierMicrozed)
        .value("kCarrierATFC", kCarrierATFC)
        .value("kCarrierAFC", kCarrierAFC)
        .value("kCarrierNexusVideo", kCarrierNexusVideo)
        .value("kCarrierTrenzTE0712", kCarrierTrenzTE0712)
        .value("kCarrierUnknown", kCarrierUnknown)
        .export_values();

    py::enum_<DesignType>(m, "DesignType")
        .value("kDesignMaster", kDesignMaster)
        .value("kDesignOuroborosSim", kDesignOuroborosSim)
        .value("kDesignOuroboros", kDesignOuroboros)
        .value("kDesignTest", kDesignTest)
        .value("kDesignEndpoint", kDesignEndpoint)
        .value("kDesignFanout", kDesignFanout)
        .value("kDesignOverlord", kDesignOverlord)
        .value("kDesignEndpoBICRT", kDesignEndpoBICRT)
        .value("kDesignChronos", kDesignChronos)
        .value("kDesignBoreas", kDesignBoreas)
        .value("kDesignKerberos", kDesignKerberos)
        .value("kDesignGaia", kDesignGaia)
        .value("kDesignUnknown", kDesignUnknown)
        .export_values();

    py::enum_<FixedLengthCommandType>(m, "FixedLengthCommandType")
        .value("TimeSync", TimeSync)
        .value("Echo", Echo)
        .value("SpillStart", SpillStart)
        .value("SpillStop", SpillStop)
        .value("RunStart", RunStart)
        .value("RunStop", RunStop)
        .value("WibCalib", WibCalib)
        .value("SSPCalib", SSPCalib)
        .value("FakeTrig0", FakeTrig0)
        .value("FakeTrig1", FakeTrig1)
        .value("FakeTrig2", FakeTrig2)
        .value("FakeTrig3", FakeTrig3)
        .value("BeamTrig", BeamTrig)
        .value("NoBeamTrig", NoBeamTrig)
        .value("ExtFakeTrig", ExtFakeTrig)
        .export_values();

    py::enum_<ClockSource>(m, "ClockSource")
        .value("kFreeRun", kFreeRun)
        .value("kInput0", kInput0)
        .value("kInput1", kInput1)
        .value("kInput2", kInput2)
        .value("kInput3", kInput3)
        .export_values();

    py::enum_<TimestampSource>(m, "TimestampSource")
        .value("kUpstream", kUpstream)
        .value("kSoftware", kSoftware)
        .value("kMixed", kMixed)
        .export_values();

    py::enum_<IRIGEpoch>(m, "IRIGEpoch")
        .value("kTAI", kTAI)
        .value("kUNIX", kUNIX)
        .export_values();

	m.attr("kBoardNameMap") = timing::IONode::get_board_type_map();
	m.attr("kCarrierNameMap") = timing::IONode::get_carrier_type_map();
	m.attr("kDesignNameMap") = timing::IONode::get_design_type_map();
	m.attr("kBoardRevisionMap") = timing::IONode::get_board_revision_map();
	m.attr("kUIDRevisionMap") = timing::IONode::get_board_uid_revision_map();
	m.attr("kClockConfigMap") = timing::IONode::get_clock_config_map();
	m.attr("kEpStates") = EndpointNode::get_endpoint_state_map();
	m.attr("kLibrarySupportedBoards") = timing::IONode::get_library_supported_boards();
    m.attr("kLibrarySupportedDesigns") = timing::IONode::get_library_supported_designs();
    m.attr("kMasterFWMajorRequired") = MasterNode::required_major_firmware_version;
}

} // namespace python
} // namespace timing
} // namespace dunedaq
