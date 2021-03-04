/**
 * @file TimingSystemManagerBase.hpp
 *
 * TimingSystemManagerBase is a class for timing system with fanout
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGSYSTEMWITHFANOUTMANAGER_HPP_
#define	TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGSYSTEMWITHFANOUTMANAGER_HPP_
 

#include "pdt/TimingSystemManager.hpp"

#include "uhal/DerivedNode.hpp"

#include <vector>
#include <string>

namespace dunedaq {
namespace pdt {

template <class MST_TOP, class EPT_TOP, class FAN_TOP>
class TimingSystemWithFanoutManager : public TimingSystemManager<MST_TOP,EPT_TOP> {

public:
    explicit TimingSystemWithFanoutManager(std::string cf);
    virtual ~TimingSystemWithFanoutManager();
    
    /**
     * @brief      Get number of fanout hardware devices
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t getNumberOfFanouts() const;

    /**
     * @brief      Get timing fanout top design node
     *
     * @return     { description_of_the_return_value }
     */
    const FAN_TOP& getFanout(uint32_t aFanoutId) const;

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
    void resetPartition(uint32_t partition_id) const override;

    /**
     * @brief      Configure and enable timing system partition
     */
    void configurePartition(uint32_t partition_id, uint32_t trigger_mask, bool enableSpillGate) const override;

    /**
     * @brief      Start timing system partition
     */
    void startPartition(uint32_t partition_id) const override;

    /**
     * @brief      Stop timing system partition
     */
    void stopPartition(uint32_t partition_id) const override;

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
    uint64_t measure_endpoint_rtt(uint32_t address) const override;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t measure_endpoint_rtt(uint32_t address, int32_t aFanout, uint32_t aMux) const;


    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t measure_endpoint_rtt(const ActiveEndpointConfig& ept_config) const override;

protected:
    std::vector<std::string> fanoutHardwareNames;
    std::vector<uhal::HwInterface> fanoutHardware;
};

} // namespace pdt
} // namespace dunedaq

#include "detail/TimingSystemWithFanoutManager.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMINGSYSTEMWITHFANOUTMANAGER_HPP_

