/**
 * @file TimestampGeneratorNode.hpp
 *
 * TimestampGeneratorNode is a class providing an interface
 * to timestamp generator firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMESTAMPGENERATORNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMESTAMPGENERATORNODE_HPP_

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "TimingIssues.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>

namespace dunedaq {
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
    std::string get_status(bool aPrint=false) const override;

    /**
     * @brief      Read the current timestamp words.
     *
     * @return     { description_of_the_return_value }
     */
    uhal::ValVector<uint32_t> read_raw_timestamp(bool aDispatch=true) const;

    /**
     * @brief      Read the current timestamp words.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t read_timestamp() const;

    /**
     * @brief      Read the current timestamp words.
     */
    void set_timestamp(uint64_t aTimestamp) const;
};


} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TIMESTAMPGENERATORNODE_HPP_