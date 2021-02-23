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
    TopDesign(const uhal::Node& node);
    virtual ~TopDesign();

    /**
     * @brief      Reset timing node.
     */
    virtual void soft_reset() const;

    /**
     * @brief      Reset timing node.
     */
    virtual void reset(const std::string& clock_config_file="") const;
    
    /**
     * @brief      Return the timing IO node.
     *
     * @return     { description_of_the_return_value }
     */
    virtual const IO& get_io_node() const;

    /**
     * @brief      Return the timing endpoint node.
     *
     * @return     { description_of_the_return_value }
     */
    virtual const EndpointNode& get_endpoint_node(uint32_t ept_id) const;

    /**
     * @brief      Print hardware information
     */
    virtual std::string get_hardware_info(bool print_out=false) const;

    /**
     * @brief      Return the timing endpoint node.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t get_number_of_endpoint_nodes() const;
};

} // namespace pdt
} // namespace dunedaq

#include "pdt/detail/TopDesign.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HPP_