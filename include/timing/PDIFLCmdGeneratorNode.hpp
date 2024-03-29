/**
 * @file PDIFLCmdGeneratorNode.hpp
 *
 * PDIFLCmdGeneratorNode is a class providing an interface
 * to the fixed length command generator firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_PDIFLCMDGENERATORNODE_HPP_
#define TIMING_INCLUDE_TIMING_PDIFLCMDGENERATORNODE_HPP_

// PDT Headers
#include "timing/definitions.hpp"
#include "timing/TimestampGeneratorNode.hpp"
#include "timing/FLCmdGeneratorNode.hpp"
#include "timing/timingfirmwareinfo/InfoStructs.hpp"
#include "timing/timingfirmwareinfo/InfoNljs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include <nlohmann/json.hpp>

#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for master global node.
 */
class PDIFLCmdGeneratorNode : public FLCmdGeneratorNode
{
  UHAL_DERIVEDNODE(PDIFLCmdGeneratorNode)
public:
  explicit PDIFLCmdGeneratorNode(const uhal::Node& node);
  virtual ~PDIFLCmdGeneratorNode();

  /**
   * @brief     Send a fixed length command
   */
  void send_fl_cmd(FixedLengthCommandType command,
                   uint32_t channel, // NOLINT(build/unsigned)
                   const TimestampGeneratorNode& timestamp_gen_node) const;

  /**
   * @brief     Send a fixed length command
   */
  void send_fl_cmd(FixedLengthCommandType command,
                   uint32_t channel) const; // NOLINT(build/unsigned)

  static const std::map<FixedLengthCommandType, std::string>& get_command_map() {return command_map;}

  static inline const size_t number_of_fl_cmds = get_command_map().size();
private:
  void validate_channel(uint32_t channel) const;

  static inline const std::map<FixedLengthCommandType, std::string> command_map = {
    { TimeSync, "TimeSync" }, { Echo, "Echo" }, { SpillStart, "SpillStart" }, { SpillStop, "SpillStop" }, { RunStart, "RunStart" },
    { RunStop, "RunStop" }, { WibCalib, "WibCalib" }, { SSPCalib, "SSPCalib" }, { FakeTrig0, "FakeTrig0" }, { FakeTrig1, "FakeTrig1" },
    { FakeTrig2, "FakeTrig2" }, { FakeTrig3, "FakeTrig3" }, { BeamTrig, "BeamTrig" }, { NoBeamTrig, "NoBeamTrig" }, { ExtFakeTrig, "ExtFakeTrig" }
  };
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_PDIFLCMDGENERATORNODE_HPP_