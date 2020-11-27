#ifndef __PDT_CRTNODE_HPP__
#define __PDT_CRTNODE_HPP__

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "pdt/exception.hpp"

namespace pdt {

/**
 * @brief      Base class for timing IO nodes.
 */
class CRTNode : public TimingNode {
    UHAL_DERIVEDNODE(CRTNode);
public:
    CRTNode(const uhal::Node& aNode);
    ~CRTNode();

    /**
     * @brief     Get status string, optionally print.
     */
    std::string getStatus(bool aPrint=false) const override;

    /**
     * @brief      Enable the crt endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void enable(uint32_t aPartition, uint32_t aCmd) const;

    /**
     * @brief      Disable the crt endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void disable() const;

    /**
     * @brief      Read the timestamp of the last pulse.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t readLastPulseTimestamp() const;

};


} // namespace pdt

#endif // __PDT_CRTTNODE_HPP__