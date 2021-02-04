#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FREQUENCYCOUNTERNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FREQUENCYCOUNTERNODE_HPP_

// C++ Headers
#include <chrono>
#include <thread>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "TimingIssues.hpp"

namespace pdt {

/**
 * @brief      Base class for timing IO nodes.
 */
class FrequencyCounterNode : public TimingNode {
    UHAL_DERIVEDNODE(FrequencyCounterNode)
public:
    FrequencyCounterNode(const uhal::Node& aNode);
    virtual ~FrequencyCounterNode();
	
	/**
     * @brief     Get status string, optionally print.
     */
    std::string getStatus(bool aPrint=false) const override;//TODO something here

    /**
     * @brief     Measure clock frequencies.
     *
     * @return     { description_of_the_return_value }
     */
    std::vector<double> measureFrequencies(uint8_t nClocks) const;

};


} // namespace pdt

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FREQUENCYCOUNTERNODE_HPP_