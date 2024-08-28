/**
 * @file EndpointNode.hpp
 *
 * EndpointNode is a class providing an interface
 * to the endpoint wrapper firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_ENDPOINTNODE_HPP_
#define TIMING_INCLUDE_TIMING_ENDPOINTNODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/FrequencyCounterNode.hpp"
#include "timing/EndpointNodeInterface.hpp"

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
class EndpointNode : public EndpointNodeInterface
{
  UHAL_DERIVEDNODE(EndpointNode)
public:
  explicit EndpointNode(const uhal::Node& node);
  virtual ~EndpointNode();

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
   * @brief      Read the endpoint clock frequency.
   *
   * @return     { description_of_the_return_value }
   */
  //virtual double read_clock_frequency() const;

  /**
   * @brief     Collect monitoring information for timing endpoint
   *
   */
  void get_info(timingendpointinfo::TimingEndpointInfo& mon_data) const override;

  /**
   * @brief    Get the states map
   */
  static std::map<uint8_t, std::string> get_endpoint_state_map() { return endpoint_state_map; }

protected:
  static inline const std::map<uint8_t, std::string> endpoint_state_map
  {
    { 0x0, "Standing by (0x0)" },                          // 0b0000 when ST_RESET, -- Endpoint in in reset
    { 0x1, "Waiting for input clock (0x1)" },              // 0b0001 when ST_W_CLK, -- Waiting for input clock
    { 0x2, "Waiting for good frequency check (0x2)" },     // 0b0010 when ST_W_FREQ, -- Waiting for frequency check
    { 0x3, "Waiting for internal CDR to lock (0x3)" },     // 0b0011 when ST_W_CDR, -- Waiting for internal CDR to lock
    { 0x4, "Waiting for rx block to lock (0x4)" },         // 0b0100 when ST_W_RX, -- Waiting for rx block to lock
    { 0x5, "Waiting for address to be set (0x5)" },        // 0b0101 when ST_W_ADDR, -- Waiting for address to be set
    { 0x6, "Waiting for deskew to be set (0x6)" },         // 0b0110 when ST_W_DESKEW, -- Waiting for deskew to be set
    { 0x7, "Waiting for timestamp initialisation (0x7)" }, // 0b0111 when ST_W_TS, -- Waiting for timestamp initialisation
    { 0x8, "Ready (0x8)" },                                // 0b1000 when ST_READY, -- Good to go
    { 0xc, "Error in physical layer (0xc)" },              // 0b1100 when ST_ERR_P, -- Error in physical layer
    { 0xd, "Receive error (0xd)" },                        // 0b1101 when ST_ERR_R, -- Receive error
    { 0xe, "Time check error (0xe)" },                     // 0b1110 when ST_ERR_T, -- Time check error
    { 0xf, "Protocol error (0xf)" },                       // 0b1111 when ST_ERR_X; -- Protocol error
  };
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_ENDPOINTNODE_HPP_
