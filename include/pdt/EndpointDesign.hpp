/**
 * @file EndpointDesign.hpp
 *
 * EndpointDesign is a class providing an interface
 * to the endpoint firmware design.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ENDPOINTDESIGN_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ENDPOINTDESIGN_HPP_

// PDT Headers
#include "pdt/TopDesign.hpp"
#include "pdt/FMCIONode.hpp"
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
 * @brief      Base class for timing endpoint design nodes.
 */
template <class IO>
class EndpointDesign : public TopDesign<IO> {
// In leiu of UHAL_DERIVEDNODE
protected:
	virtual uhal::Node* clone() const;
//
public:
    EndpointDesign(const uhal::Node& aNode);
    virtual ~EndpointDesign();
	
	/**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool aPrint=false) const override;

    /**
     * @brief      Enable the specified endpoint node.
     *
     * @return     { description_of_the_return_value }
     */
    void enable(uint32_t aEptId) const;

};

} // namespace pdt
} // namespace dunedaq

#include "pdt/detail/EndpointDesign.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ENDPOINTDESIGN_HPP_