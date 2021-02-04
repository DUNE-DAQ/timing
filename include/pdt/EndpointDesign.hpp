#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ENDPOINTDESIGN_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ENDPOINTDESIGN_HPP_

// C++ Headers
#include <chrono>
#include <string>
#include <sstream>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TopDesign.hpp"
#include "pdt/FMCIONode.hpp"
#include "TimingIssues.hpp"

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
    std::string getStatus(bool aPrint=false) const override;

    /**
     * @brief      Enable the specified endpoint node.
     *
     * @return     { description_of_the_return_value }
     */
    void enable(uint32_t aEptId) const;

};

} // namespace pdt

#include "pdt/EndpointDesign.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_ENDPOINTDESIGN_HPP_