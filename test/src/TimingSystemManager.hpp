/**
 * @file TimingSystemManagerBase.hpp
 *
 * TimingSystemManagerBase is a class for timing system without fanout
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_TEST_SRC_TIMINGSYSTEMMANAGER_HPP_
#define TIMING_TEST_SRC_TIMINGSYSTEMMANAGER_HPP_

#include "TimingIssues.hpp"
#include "timing/TimingSystemManagerBase.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <string>
#include <vector>

namespace dunedaq {
namespace timing {
/**
 * @class TimingSystemManager
 * @brief Brief
 *
 * Description
 * @author Stoyan Trilov
 * @date September 2020
 *
 */

template<class MST_TOP, class EPT_TOP>
class TimingSystemManager : public TimingSystemManagerBase
{
public:
  explicit TimingSystemManager(std::string cf);
  virtual ~TimingSystemManager();

  /**
   * @brief      Get timing master top design node
   *
   * @return     { description_of_the_return_value }
   */
  const MST_TOP& getMaster(uint32_t aMstId) const; // NOLINT(build/unsigned)

  /**
   * @brief      Get timing endpoint top design node
   *
   * @return     { description_of_the_return_value }
   */
  const EPT_TOP& getEndpoint(uint32_t endpoint_id) const; // NOLINT(build/unsigned)

  /**
   * @brief     Print the status of the timing node.
   */
  void printSystemStatus() const override;

  /**
   * @brief      Reset timing system hardware
   */
  void resetSystem() const override;

  /**
   * @brief      Configure timing system hardware
   */
  void configureSystem() const override;

  /**
   * @brief      Reset timing system partition
   */
  void resetPartition(uint32_t partition_id) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Configure and enable timing system partition
   */
  void configurePartition(uint32_t partition_id, // NOLINT(build/unsigned)
                          uint32_t trigger_mask, // NOLINT(build/unsigned)
                          bool enableSpillGate) const override;

  /**
   * @brief      Start timing system partition
   */
  void startPartition(uint32_t partition_id) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Stop timing system partition
   */
  void stopPartition(uint32_t partition_id) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Read the current timestamp
   *
   * @return     { description_of_the_return_value }
   */
  uint64_t read_master_timestamp() const override; // NOLINT(build/unsigned)

  /**
   * @brief      Measure the endpoint round trip time.
   *
   * @return     { description_of_the_return_value }
   */
  uint64_t measure_endpoint_rtt(uint32_t address) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Measure the endpoint round trip time.
   *
   * @return     { description_of_the_return_value }
   */
  uint64_t measure_endpoint_rtt(const ActiveEndpointConfig& ept_config) const override; // NOLINT(build/unsigned)

  /**
   * @brief      Measure the round trip time for endpoints.
   */
  std::vector<EndpointRTTResult> performEndpointRTTScan() override;

  /**
   * @brief      Adjust the endpoint delays.
   */
  void apply_endpoint_delays(uint32_t measure_rtt) const override; // NOLINT(build/unsigned)
};

} // namespace timing
} // namespace dunedaq

#include "detail/TimingSystemManager.hxx"

#endif // TIMING_TEST_SRC_TIMINGSYSTEMMANAGER_HPP_
