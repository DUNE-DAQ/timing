#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMESTAMPGENERATORNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMESTAMPGENERATORNODE_HPP_

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "TimingIssues.hpp"

namespace pdt {

/**
 * @brief      Class for timestamp generator node.
 */
class TimestampGeneratorNode : public TimingNode {
    UHAL_DERIVEDNODE(TimestampGeneratorNode)
public:
    TimestampGeneratorNode(const uhal::Node& aNode);
    virtual ~TimestampGeneratorNode();
    
    /**
     * @brief     Print the status of the timing node.
     */
    std::string getStatus(bool aPrint=false) const override;

    /**
     * @brief      Read the current timestamp words.
     *
     * @return     { description_of_the_return_value }
     */
    uhal::ValVector<uint32_t> readRawTimestamp(bool aDispatch=true) const;

    /**
     * @brief      Read the current timestamp words.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t readTimestamp() const;

    /**
     * @brief      Read the current timestamp words.
     */
    void setTimestamp(uint64_t aTimestamp) const;
};


} // namespace pdt

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMESTAMPGENERATORNODE_HPP_