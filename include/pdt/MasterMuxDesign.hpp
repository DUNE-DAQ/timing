/**
 * @file MasterMuxDesign.hpp
 *
 * MasterDesign is a base class providing an interface
 * to for top level master firmware designs on boards with muxes.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERMUXDESIGN_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERMUXDESIGN_HPP_

// PDT Headers
#include "pdt/MasterDesign.hpp"
#include "pdt/PC059IONode.hpp"
#include "pdt/PDIMasterNode.hpp"

#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>
#include <sstream>

namespace dunedaq {
namespace pdt {

/**
 * @brief      Class for timing fanout designs.
 */
template <class IO, class MST>
class MasterMuxDesign : public MasterDesign<IO,MST> {
// In leiu of UHAL_DERIVEDNODE
protected:
    virtual uhal::Node* clone() const;
//
public:
    MasterMuxDesign(const uhal::Node& aNode);
    virtual ~MasterMuxDesign();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool aPrint=false) const override;
    
    /**
     * @brief      Prepare the timing fanout for data taking.
     *
     */
    void configure() const override;

    /**
     * @brief      Reset timing fanout node.
     */
    void reset(const std::string& aClockConfigFile="") const override;

    /**
     * @brief     Switch the SFP mux channel
     */
    virtual void switchSFPMUXChannel(uint32_t aSFPID, bool aWaitForRttEptLock) const;

    /**
     * @brief     Read the active SFP mux channel
     */
    virtual uint32_t readActiveSFPMUXChannel() const;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t measureEndpointRTT(uint32_t aAddr, bool aControlSFP, uint32_t aSFPMUX) const;

    /**
     * @brief      Apply delay to endpoint
     */
    virtual void applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT, bool aControlSFP, uint32_t aSFPMUX) const;

    /**
     * @brief      Apply delay to endpoint
     */
    virtual std::vector<uint32_t> scanSFPMUX() const;
};

} // namespace pdt
} // namespace dunedaq

#include "pdt/detail/MasterMuxDesign.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERMUXDESIGN_HPP_