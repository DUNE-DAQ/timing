/**
 * @file EchoMonitorNode.hpp
 *
 * EchoMonitorNode is a class providing an interface
 * to the echo monitor firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ECHOMONITORNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ECHOMONITORNODE_HPP_

// PDT Headers
#include "pdt/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

namespace dunedaq {
namespace pdt {

/**
 * @brief      Class for master global node.
 */
class EchoMonitorNode : public TimingNode {
    UHAL_DERIVEDNODE(EchoMonitorNode)
public:
    EchoMonitorNode(const uhal::Node& aNode);
    virtual ~EchoMonitorNode();

    /**
     * @brief      Send echo and measure the round-trip time between master and endpoint
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t sendEchoAndMeasureDelay(int64_t aTimeout=500) const;

    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool aPrint=false) const override;
};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ECHOMONITORNODE_HPP_