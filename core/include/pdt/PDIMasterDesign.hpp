#ifndef __PDT_PDIMASTERDESIGN_HPP__
#define __PDT_PDIMASTERDESIGN_HPP__

// C++ Headers
#include <chrono>
#include <string>
#include <sstream>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/MasterDesign.hpp"
#include "pdt/TLUIONode.hpp"
#include "pdt/FMCIONode.hpp"
#include "pdt/PDIMasterNode.hpp"
#include "pdt/SIMIONode.hpp"

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
    std::string getStatus(bool aPrint=false) const override;

    /**
     * @brief      Prepare the timing master for data taking.
     *
     */
    void configure() const override;

};

} // namespace pdt

#include "pdt/PDIMasterDesign.hxx"

#endif // __PDT_PDIMASTERDESIGN_HPP__