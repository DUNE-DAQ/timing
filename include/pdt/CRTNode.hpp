/**
 * @file CRTNode.hpp
 *
 * CRTNode is a class providing an interface
 * to the crt wrapper firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_CRTNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_CRTNODE_HPP_

#include "pdt/TimingNode.hpp"
#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>

namespace dunedaq {
namespace pdt {

/**
 * @brief      Base class for timing IO nodes.
 */
class CRTNode : public TimingNode {
    UHAL_DERIVEDNODE(CRTNode)
public:
    CRTNode(const uhal::Node& aNode);
    ~CRTNode();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool aPrint=false) const override;

    /**
     * @brief      Enable the crt endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void enable(uint32_t aPartition, uint32_t aCmd) const;

    /**
     * @brief      Disable the crt endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void disable() const;

    /**
     * @brief      Read the timestamp of the last pulse.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t read_last_pulse_timestamp() const;

};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_CRTTNODE_HPP_