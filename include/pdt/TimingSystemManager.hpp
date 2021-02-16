/**
 * @file    TimingSystemManager.hpp
 * @author  Stoyan Trilov
 * @brief   Brief description
 * @date 
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGSYSTEMMANAGER_HPP_
#define	TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGSYSTEMMANAGER_HPP_

#include <vector>
 
// uHal Headers
#include "uhal/DerivedNode.hpp"

#include "TimingIssues.hpp"
#include "pdt/TimingSystemManagerBase.hpp"

namespace dunedaq {
namespace pdt {
/*!
 * @class TimingSystemManager
 * @brief Brief
 *
 * Description
 * @author Stoyan Trilov
 * @date September 2020
 *
 */

template <class MST_TOP, class EPT_TOP>
class TimingSystemManager : public TimingSystemManagerBase {
public:
    TimingSystemManager(std::string cf);
    virtual ~TimingSystemManager();
    
    /**
     * @brief      Get timing master top design node
     *
     * @return     { description_of_the_return_value }
     */
    const MST_TOP& getMaster(uint32_t aMstId) const;

    /**
     * @brief      Get timing endpoint top design node
     *
     * @return     { description_of_the_return_value }
     */
    const EPT_TOP& getEndpoint(uint32_t aEptId) const;

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
    void resetPartition(uint32_t aPartID) const override;

    /**
     * @brief      Configure and enable timing system partition
     */
    void configurePartition(uint32_t aPartID, uint32_t aTrigMask, bool aEnableSpillGate) const override;

    /**
     * @brief      Start timing system partition
     */
    void startPartition(uint32_t aPartID) const override;

    /**
     * @brief      Stop timing system partition
     */
    void stopPartition(uint32_t aPartID) const override;

    /**
     * @brief      Read the current timestamp
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t read_master_timestamp() const override;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t measureEndpointRTT(uint32_t aAddr) const override;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t measureEndpointRTT(const ActiveEndpointConfig& aEptConfig) const override;

    /**
     * @brief      Measure the round trip time for endpoints.
     */
    std::vector<EndpointRTTResult> performEndpointRTTScan() override;

    /**
     * @brief      Adjust the endpoint delays.
     */
    void apply_endpoint_delays(uint32_t aMeasureRTT) const override;
};

} // namespace pdt
} // namespace dunedaq

#include "detail/TimingSystemManager.hxx"

#endif	// TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGSYSTEMMANAGER_HPP_

