/**
 * @file PDIEndpointNode.hpp
 *
 * PDIEndpointNode is a class providing an interface
 * to the endpoint wrapper firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_PDIENDPOINTNODE_HPP_
#define TIMING_INCLUDE_TIMING_PDIENDPOINTNODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/FrequencyCounterNode.hpp"
#include "timing/EndpointNodeInterface.hpp"

#include "timing/timingendpointinfo/InfoNljs.hpp"
#include "timing/timingendpointinfo/InfoStructs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <nlohmann/json.hpp>

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing IO nodes.
 */
class PDIEndpointNode : public EndpointNodeInterface
{
  UHAL_DERIVEDNODE(PDIEndpointNode)
public:
  explicit PDIEndpointNode(const uhal::Node& node);
  virtual ~PDIEndpointNode();

  /**
   * @brief     Print the status of the timing node.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Enable the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  void enable(uint32_t address = 0, uint32_t partition = 0) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Disable the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  void disable() const override;

  /**
   * @brief      Reset the endpoint
   *
   * @return     { description_of_the_return_value }
   */
  void reset(uint32_t address = 0, uint32_t partition = 0) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Read the current timestamp word.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint64_t read_timestamp() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the number of words in the data buffer.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_buffer_count() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the contents of the endpoint data buffer.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uhal::ValVector<uint32_t> read_data_buffer(bool read_all = false) const; // NOLINT(build/unsigned)

  /**
   * @brief      Print the contents of the endpoint data buffer.
   *
   * @return     { description_of_the_return_value }
   */
  virtual std::string get_data_buffer_table(bool read_all = false, bool print_out = false) const;

  /**
   * @brief      Read the endpoint clock frequency.
   *
   * @return     { description_of_the_return_value }
   */
  virtual double read_clock_frequency() const;

  /**
   * @brief      Read the endpoint wrapper version
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_version() const; // NOLINT(build/unsigned)

  /**
   * @brief     Collect monitoring information for timing endpoint
   *
   */
  virtual void get_info(timingendpointinfo::TimingEndpointInfo& mon_data) const;

  /**
   * @brief    Give info to collector.
   */
  void get_info(opmonlib::InfoCollector& ci, int level) const override;

  /**
   * @brief    Get the states map
   */
  static std::map<uint8_t, std::string> get_endpoint_state_map() { return endpoint_state_map; }
protected:
  static inline const std::map<uint8_t, std::string> endpoint_state_map
  {
    { 0x0, "Standing by (0x0)" },                              // 0b0000 when W_RST, -- Starting state after reset
    { 0x1, "Waiting for SFP signal (0x1)" },                   // 0b0001 when W_LINK, -- Waiting for SFP LOS to go low
    { 0x2, "Waiting for good frequency check (0x2)" },         // 0b0010 when W_FREQ, -- Waiting for good frequency check
    { 0x3, "Waiting for phase adjustment to complete (0x3)" }, // 0b0011 when W_ADJUST, -- Waiting for phase adjustment to complete
    { 0x4, "Waiting for comma alignment (0x4)" },              // 0b0100 when W_ALIGN, -- Waiting for comma alignment, stable clock phase
    { 0x5, "Waiting for 8b10 decoder good packet (0x5)" },     // 0b0101 when W_LOCK, -- Waiting for 8b10 decoder good packet
    { 0x6, "Waiting for phase adjustment command (0x6)" },     // 0b0110 when W_PHASE, -- Waiting for phase adjustment command
    { 0x7, "Waiting for time stamp initialisation (0x7)" },    // 0b0111 when W_RDY, -- Waiting for time stamp initialisation
    { 0x8, "Ready (0x8)" },                                    // 0b1000 when RUN, -- Good to go
    { 0xc, "Error in Rx (0xc)" },                              // 0b1100 when ERR_R, -- Error in rx
    { 0xd, "Error in time stamp check (0xd)" },                // 0b1101 when ERR_T; -- Error in time stamp check
    { 0xe, "Error in physical layer after lock (0xe)" },       // 0b1110 when ERR_P; -- Physical layer error after lock
  };
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_PDIENDPOINTNODE_HPP_