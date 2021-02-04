#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HPP_

// C++ Headers
#include <chrono>
#include <string>
#include <sstream>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "pdt/EndpointNode.hpp"
#include "TimingIssues.hpp"

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

#include "pdt/TopDesign.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOPDESIGN_HPP_