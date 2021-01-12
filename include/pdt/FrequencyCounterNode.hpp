#ifndef __PDT_FREQUENCYCOUNTERNODE_HPP__
#define __PDT_FREQUENCYCOUNTERNODE_HPP__

// C++ Headers
#include <chrono>
#include <thread>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "pdt/exception.hpp"

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

#endif // __PDT_FREQUENCYCOUNTERNODE_HPP__