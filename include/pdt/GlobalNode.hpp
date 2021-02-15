/**
 * @file GlobalNode.hpp
 *
 * GlobalNode is a class providing an interface
 * to the master global firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_GLOBALNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_GLOBALNODE_HPP_

// PDT Headers
#include "pdt/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

namespace dunedaq {
namespace pdt {

/**
 * @brief      Class for master global node.
 */
class GlobalNode : public TimingNode {
    UHAL_DERIVEDNODE(GlobalNode)
public:
    GlobalNode(const uhal::Node& aNode);
    virtual ~GlobalNode();

    bool in_spill() const;
    bool tx_error() const;

    uint32_t readTimeStamp() const;
    uint32_t readSpillCounter() const;

    void selectPartition() const;
    void lockPartition() const;

    /**
     * @brief     Enable the upstream endpoint.
     */
    void enableUpstreamEndpoint(uint32_t aTimeout=500);

    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool aPrint=false) const override;
};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_GLOBALNODE_HPP_