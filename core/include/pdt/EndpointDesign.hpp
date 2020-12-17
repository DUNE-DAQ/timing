#ifndef __PDT_ENDPOINTDESIGN_HPP__
#define __PDT_ENDPOINTDESIGN_HPP__

// C++ Headers
#include <chrono>
#include <string>
#include <sstream>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TopDesign.hpp"
#include "pdt/FMCIONode.hpp"
#include "pdt/FIBIONode.hpp"
#include "pdt/exception.hpp"

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

#endif // __PDT_ENDPOINTDESIGN_HPP__