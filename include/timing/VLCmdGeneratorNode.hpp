/**
 * @file VLCmdGeneratorNode.hpp
 *
 * VLCmdGeneratorNode is a class providing an interface
 * to the variable length command generator firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_VLCMDGENERATORNODE_HPP_
#define TIMING_INCLUDE_TIMING_VLCMDGENERATORNODE_HPP_

// PDT Headers
#include "timing/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

#include <string>

namespace dunedaq {
namespace timing {

/**
 * @brief      Class for master global node.
 */
class VLCmdGeneratorNode : public TimingNode {
    UHAL_DERIVEDNODE(VLCmdGeneratorNode)
public:
    explicit VLCmdGeneratorNode(const uhal::Node& node);
    virtual ~VLCmdGeneratorNode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string get_status(bool print_out=false) const override;
    
    /**
     * @brief     Control the endpoint sfp tx laser.
     */
    void switch_endpoint_sfp(uint32_t address, bool enable=false) const;

    /**
     * @brief     Adjust endpoint delay.
     */
    void apply_endpoint_delay(uint32_t address, uint32_t coarse_delay, uint32_t fine_delay, uint32_t phase_delay) const;
};

} // namespace timing
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_VLCMDGENERATORNODE_HPP_