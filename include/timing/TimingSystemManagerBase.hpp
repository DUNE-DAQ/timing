/**
 * @file TimingSystemManagerBase.hpp
 *
 * TimingSystemManagerBase is a base class for timing manger classes
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_TIMINGSYSTEMMANAGERBASE_HPP_
#define	TIMING_INCLUDE_TIMING_TIMINGSYSTEMMANAGERBASE_HPP_
 
#include "TimingIssues.hpp"
#include "timing/FMCIONode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include "uhal/ConnectionManager.hpp"

#include <vector>
#include <map>
#include <string>

namespace dunedaq {
namespace timing {

class TimingSystemManagerBase : boost::noncopyable {

public:
    explicit TimingSystemManagerBase(std::string cf);
    virtual ~TimingSystemManagerBase();

    /**
     * @brief      Get number of master hardware devices
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t getNumberOfMasters() const;

    /**
     * @brief      Get number of endpoint hardware devices
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t getNumberOfEndpoints() const;
    
    /**
     * @brief     Print the status of the timing node.
     */
    virtual void printSystemStatus() const = 0;

    /**
     * @brief      Reset timing system hardware
     */
    virtual void resetSystem() const = 0;

    /**
     * @brief      Configure timing system hardware
     */
    virtual void configureSystem() const = 0;

    /**
     * @brief      Reset timing system partition
     */
    virtual void resetPartition(uint32_t partition_id) const = 0;

    /**
     * @brief      Configure timing system partition
     */
    virtual void configurePartition(uint32_t partition_id, uint32_t trigger_mask, bool enableSpillGate) const = 0;

    /**
     * @brief      Start timing system partition
     */
    virtual void startPartition(uint32_t partition_id) const = 0;

    /**
     * @brief      Stop timing system partition
     */
    virtual void stopPartition(uint32_t partition_id) const = 0;

    /**
     * @brief      Read the current timestamp
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t read_master_timestamp() const = 0;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t measure_endpoint_rtt(uint32_t address) const = 0;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t measure_endpoint_rtt(const ActiveEndpointConfig& ept_config) const = 0;

    /**
     * @brief      Measure the round trip time for endpoints.
     */
    virtual std::vector<EndpointRTTResult> performEndpointRTTScan() = 0;

    /**
     * @brief      Adjust the endpoint delays.
     */
    virtual void apply_endpoint_delays(uint32_t measure_rtt) const = 0;

protected:

    const std::string connectionsFile;
    uhal::ConnectionManager* connectionManager;

    std::vector<std::string> masterHardwareNames;
    std::vector<std::string> endpointHardwareNames;

    std::vector<uhal::HwInterface> masterHardware;
    std::vector<uhal::HwInterface> endpointHardware;

    std::map<std::string, ActiveEndpointConfig> mExpectedEndpoints;

    uint32_t mMaxMeasuredRTT;
};

} // namespace timing
} // namespace dunedaq

#endif	// TIMING_INCLUDE_TIMING_TIMINGSYSTEMMANAGERBASE_HPP_

