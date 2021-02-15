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

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_VLCMDGENERATORNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_VLCMDGENERATORNODE_HPP_

// PDT Headers
#include "pdt/TimingNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

namespace dunedaq {
namespace pdt {

/**
 * @brief      Class for master global node.
 */
class VLCmdGeneratorNode : public TimingNode {
    UHAL_DERIVEDNODE(VLCmdGeneratorNode)
public:
    VLCmdGeneratorNode(const uhal::Node& aNode);
    virtual ~VLCmdGeneratorNode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string get_status(bool aPrint=false) const override;
    
    /**
     * @brief     Control the endpoint sfp tx laser.
     */
    void switchEndpointSFP(uint32_t aAddr, bool aEnable=false) const;

    /**
     * @brief     Adjust endpoint delay.
     */
    void applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel) const;
};

} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_VLCMDGENERATORNODE_HPP_