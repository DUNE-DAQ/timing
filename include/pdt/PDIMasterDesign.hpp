/**
 * @file PDIMasterDesign.hpp
 *
 * PDIMasterDesign is a class providing an interface
 * to the PD-I master design firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PDIMASTERDESIGN_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PDIMASTERDESIGN_HPP_

// PDT Headers
#include "pdt/MasterDesign.hpp"
#include "pdt/TLUIONode.hpp"
#include "pdt/FMCIONode.hpp"
#include "pdt/PDIMasterNode.hpp"
#include "pdt/SIMIONode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <string>
#include <sstream>

namespace dunedaq {
namespace pdt {

/**
 * @brief      Class for PDI timing master design (known as overlord).
 */
template <class IO>
class PDIMasterDesign : public MasterDesign<IO,PDIMasterNode> {
// In leiu of UHAL_DERIVEDNODE
protected:
    virtual uhal::Node* clone() const;
//
public:
    PDIMasterDesign(const uhal::Node& aNode);
    virtual ~PDIMasterDesign();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string get_status(bool aPrint=false) const override;

    /**
     * @brief      Prepare the timing master for data taking.
     *
     */
    void configure() const override;

};

} // namespace pdt
} // namespace dunedaq

#include "pdt/detail/PDIMasterDesign.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PDIMASTERDESIGN_HPP_