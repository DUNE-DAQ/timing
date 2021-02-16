/**
 * @file FanoutDesign.hpp
 *
 * FanoutDesign is a class providing an interface
 * to the fanout firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FANOUTDESIGN_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FANOUTDESIGN_HPP_

// PDT Headers
#include "pdt/MasterMuxDesign.hpp"
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
class FanoutDesign : public MasterMuxDesign<IO,MST> {
// In leiu of UHAL_DERIVEDNODE
protected:
    virtual uhal::Node* clone() const;
//
public:
    FanoutDesign(const uhal::Node& aNode);
    virtual ~FanoutDesign();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool aPrint=false) const override;
    
    /**
     * @brief      Prepare the timing fanout for data taking.
     *
     */
    void configure() const override;

    ///**
    // * @brief      Reset timing fanout node.
    // */
    //void reset(const std::string& aClockConfigFile="") const override;

    /**
     * @brief      Reset timing fanout node.
     */
    virtual void reset(uint32_t aFanoutMode, const std::string& aClockConfigFile="") const;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t measureEndpointRTT(uint32_t aAddr, bool aControlSFP, uint32_t aSFPMUX) const override;

    /**
     * @brief      Apply delay to endpoint
     */
    virtual void apply_endpoint_delay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT, bool aControlSFP, uint32_t aSFPMUX) const override;
};

} // namespace pdt
} // namespace dunedaq

#include "pdt/detail/FanoutDesign.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FANOUTDESIGN_HPP_