/**
 * @file    TimingSystemManagerBase.hpp
 * @author  Stoyan Trilov
 * @brief   Brief description
 * @date 
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGSYSTEMMANAGERBASE_HPP_
#define	TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGSYSTEMMANAGERBASE_HPP_

#include <vector>
 
// uHal Headers
#include "uhal/DerivedNode.hpp"
#include "uhal/ConnectionManager.hpp"

#include "TimingIssues.hpp"
#include "pdt/FMCIONode.hpp"
 
namespace pdt {

class TimingSystemManagerBase : boost::noncopyable {
protected:

    const std::string connectionsFile;
    uhal::ConnectionManager* connectionManager;

    std::vector<std::string> masterHardwareNames;
    std::vector<std::string> endpointHardwareNames;

    std::vector<uhal::HwInterface> masterHardware;
    std::vector<uhal::HwInterface> endpointHardware;

    std::map<std::string, ActiveEndpointConfig> mExpectedEndpoints;

    uint32_t mMaxMeasuredRTT;
public:
    TimingSystemManagerBase(std::string cf);
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
    virtual void resetPartition(uint32_t aPartID) const = 0;

    /**
     * @brief      Configure timing system partition
     */
    virtual void configurePartition(uint32_t aPartID, uint32_t aTrigMask, bool aEnableSpillGate) const = 0;

    /**
     * @brief      Start timing system partition
     */
    virtual void startPartition(uint32_t aPartID) const = 0;

    /**
     * @brief      Stop timing system partition
     */
    virtual void stopPartition(uint32_t aPartID) const = 0;

    /**
     * @brief      Read the current timestamp
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t readMasterTimestamp() const = 0;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t measureEndpointRTT(uint32_t aAddr) const = 0;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t measureEndpointRTT(const ActiveEndpointConfig& aEptConfig) const = 0;

    /**
     * @brief      Measure the round trip time for endpoints.
     */
    virtual std::vector<EndpointRTTResult> performEndpointRTTScan() = 0;

    /**
     * @brief      Adjust the endpoint delays.
     */
    virtual void applyEndpointDelays(uint32_t aMeasureRTT) const = 0;

};

} // namespace pdt

#endif	/* TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGSYSTEMMANAGERBASE_HPP_ */

