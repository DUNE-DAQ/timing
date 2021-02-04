#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FANOUTDESIGN_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FANOUTDESIGN_HPP_

// C++ Headers
#include <chrono>
#include <string>
#include <sstream>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/MasterMuxDesign.hpp"
#include "pdt/PC059IONode.hpp"
#include "pdt/PDIMasterNode.hpp"

#include "TimingIssues.hpp"

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
    std::string getStatus(bool aPrint=false) const override;
    
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
    virtual void applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT, bool aControlSFP, uint32_t aSFPMUX) const override;
};

} // namespace pdt

#include "pdt/FanoutDesign.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FANOUTDESIGN_HPP_