/**
 * @file TopDesign.hpp
 *
 * TopDesign is a class providing the base interface
 * for timing top design nodes.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HPP_

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "pdt/EndpointNode.hpp"
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
 * @brief      Base class for timing top design nodes.
 */
template <class IO>
class TopDesign : public TimingNode {
public:
    TopDesign(const uhal::Node& aNode);
    virtual ~TopDesign();

    /**
     * @brief      Reset timing node.
     */
    virtual void softReset() const;

    /**
     * @brief      Reset timing node.
     */
    virtual void reset(const std::string& aClockConfigFile="") const;
    
    /**
     * @brief      Return the timing IO node.
     *
     * @return     { description_of_the_return_value }
     */
    virtual const IO& getIONode() const;

    /**
     * @brief      Return the timing endpoint node.
     *
     * @return     { description_of_the_return_value }
     */
    virtual const EndpointNode& getEndpointNode(uint32_t ept_id) const;

    /**
     * @brief      Print hardware information
     */
    virtual std::string getHardwareInfo(bool aPrint=false) const;

    /**
     * @brief      Return the timing endpoint node.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t getNumberOfEndpointNodes() const;
};

} // namespace pdt
} // namespace dunedaq

#include "pdt/detail/TopDesign.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HPP_