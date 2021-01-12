#ifndef __PDT_FMCIONODE_HPP__
#define __PDT_FMCIONODE_HPP__

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/IONode.hpp"
#include "pdt/exception.hpp"

namespace pdt {

/**
 * @brief      Class for the timing FMC board.
 */
class FMCIONode : public IONode {
    UHAL_DERIVEDNODE(FMCIONode)

public:
    FMCIONode(const uhal::Node& aNode);
    virtual ~FMCIONode();
    
    /**
     * @brief     Get status string, optionally print.
     */
    std::string getStatus(bool aPrint=false) const override;

    /**
     * @brief      Reset timing node.
     */
    void reset(const std::string& aClockConfigFile="") const override;

};

} // namespace pdt

#endif // __PDT_FMCIONODE_HPP__