/**
 * @file HSINode.hpp
 *
 * HSINode is a class providing an interface
 * to the HSI wrapper firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_HSINODE_HPP_
#define TIMING_INCLUDE_TIMING_HSINODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "timing/TimingNode.hpp"
#include "timing/definitions.hpp"

#include "timing/timingfirmwareinfo/InfoNljs.hpp"
#include "timing/timingfirmwareinfo/InfoStructs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for HSI nodes.
 */
class HSINode : public TimingNode
{
  UHAL_DERIVEDNODE(HSINode)
public:
  explicit HSINode(const uhal::Node& node);
  virtual ~HSINode();

  /**
   * @brief     Print the status of the timing node.
   */
  std::string get_status(bool print_out = false) const override;

  /**
   * @brief      Read the number of words in the data buffer.
   *
   * @return     { description_of_the_return_value }
   */
  uint32_t read_buffer_count() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the contents of the endpoint data buffer.
   *
   * @return     { description_of_the_return_value }
   */
  uhal::ValVector<uint32_t> read_data_buffer(uint16_t& n_words, // NOLINT(build/unsigned)
                                             bool read_all = false,
                                             bool fail_on_error = false) const;
  uhal::ValVector<uint32_t> read_data_buffer(bool read_all = false, // NOLINT(build/unsigned)
                                             bool fail_on_error = false) const;

  /**
   * @brief      Print the contents of the endpoint data buffer.
   *
   * @return     { description_of_the_return_value }
   */
  std::string get_data_buffer_table(bool read_all = false, bool print_out = false) const;

  /**
   * @brief     Configure HSI triggering
   *
   */
  void configure_hsi(uint32_t src,      // NOLINT(build/unsigned)
                     uint32_t re_mask,  // NOLINT(build/unsigned)
                     uint32_t fe_mask,  // NOLINT(build/unsigned)
                     uint32_t inv_mask, // NOLINT(build/unsigned)
                     double rate,
                     uint32_t clock_frequency_hz, // NOLINT(build/unsigned)
                     bool dispatch = true) const;

  /**
   * @brief     Start HSI triggering
   *
   */
  void start_hsi(bool dispatch = true) const;

  /**
   * @brief     Stop HSI triggering
   *
   */
  void stop_hsi(bool dispatch = true) const;

  /**
   * @brief     Reset HSI
   *
   */
  void reset_hsi(bool dispatch = true) const;

  /**
   * @brief     Read butffer warning flag
   *
   */
  bool read_buffer_warning() const;

  /**
   * @brief     Read butffer error flag
   *
   */
  bool read_buffer_error() const;

  /**
   * @brief     Read butffer error/warning/word count
   *
   */
  uint32_t read_buffer_state() const; // NOLINT(build/unsigned)

  /**
   * @brief     Read signal source, 0 - hardware, 1 - internal emulation
   *
   */
  uint32_t read_signal_source_mode() const; // NOLINT(build/unsigned)

  /**
   * @brief     Collect monitoring information for timing endpoint
   *
   */
  void get_info(timingfirmwareinfo::HSIFirmwareMonitorData& mon_data) const;

  /**
   * @brief    Give info to collector.
   */
  void get_info(opmonlib::InfoCollector& ci, int level) const override;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_HSINODE_HPP_