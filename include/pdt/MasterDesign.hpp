#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERDESIGN_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERDESIGN_HPP_

// C++ Headers
#include <chrono>
#include <string>
#include <sstream>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TopDesign.hpp"

namespace pdt {

/**
 * @brief      Base class for timing master designs.
 */
template <class IO, class MST>
class MasterDesign : public TopDesign<IO> {

public:
    MasterDesign(const uhal::Node& aNode);
    virtual ~MasterDesign();

    /**
     * @brief      Get the timing master node.
     *
     * @return     { description_of_the_return_value }
     */
    virtual const MST& getMasterNode() const;

    /**
     * @brief      Configure the timing master design node.
     */
    virtual void configure() const = 0;

    /**
     * @brief      Read the current timestamp.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t readMasterTimestamp() const;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t measureEndpointRTT(uint32_t aAddr, bool aControlSFP=true) const;

    /**
     * @brief      Apply delay to endpoint
     */
    virtual void applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT=false, bool aControlSFP=true) const;

};

} // namespace pdt

#include "pdt/MasterDesign.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_MASTERDESIGN_HPP_