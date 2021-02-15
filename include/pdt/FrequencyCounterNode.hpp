/**
 * @file FrequencyCounterNode.hpp
 *
 * FrequencyCounterNode is a class providing an interface
 * to the frequency counter firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FREQUENCYCOUNTERNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FREQUENCYCOUNTERNODE_HPP_

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <thread>

namespace dunedaq {
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
    std::string get_status(bool aPrint=false) const override;//TODO something here

    /**
     * @brief     Measure clock frequencies.
     *
     * @return     { description_of_the_return_value }
     */
    std::vector<double> measureFrequencies(uint8_t nClocks) const;

};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_FREQUENCYCOUNTERNODE_HPP_