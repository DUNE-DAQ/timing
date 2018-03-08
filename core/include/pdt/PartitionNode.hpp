#ifndef __PDT_PARTITIONNODE_HPP__
#define __PDT_PARTITIONNODE_HPP__

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/exception.hpp"

namespace pdt {


PDTExceptionClass(EventReadError, "Error while reading events from the partition");
PDTExceptionClass(RunRequestTimeoutExpired, "Timeout when waiting for a run request");

/**
 * @brief      Class for partition node.
 */
class PartitionNode : public uhal::Node {
    UHAL_DERIVEDNODE(PartitionNode);
public:
    PartitionNode(const uhal::Node& aNode);
    virtual ~PartitionNode();

    static const uint32_t kWordsPerEvent;

    /**
     * @brief      Reads a command mask.
     *
     * @return     The current value of the command mask.
     */
    uint32_t readTriggerMask() const;


    /**
     * @brief      Read the number words in buffer.
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t readBufferWordCount() const;

    /**
     * @brief      Count the number of events available in the readout buffer
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t numEventsInBuffer() const;
    
    /**
     * @brief      Read multiple events from the rob.
     *
     * @param[in]  aNumEvents  Number of events to read
     *
     * @return     Standard vector containing all events extracted from rob
     */
    std::vector<uint32_t> readEvents( size_t aNumEvents = 0 ) const;

    /**
     * @brief      Enables the partition now.
     *
     * @param[in]  aEnable  A enable
     */
    void enable( bool aEnable=true, bool aDispatch=true ) const;

    /**
     * @brief      Writes the trigger mask.
     *
     * @param[in]  aMask  A mask
     */
    void writeTriggerMask( uint32_t aMask ) const;

    /**
     * @brief      Enables the triggers.
     *
     * @param[in]  aEnable  The enable switch
     */
    void enableTriggers( bool aEnable = true ) const;

    /**
     * @brief      Resets the partition.
     *
     *             Disables the partition istelf, the readout buffer, resets the
     *             trigger and event counters.
     */
    void reset() const;

    /**
     * @brief      Starts the partition.
     *
     *             Flushes the readout buffer, set the run bit and wait for
     *             acknowledgment (to implement in v4)
     *
     * @param[in]  aTimeout  in milliseconds
     */
    void start( uint32_t aTimeout = 5000 ) const;

    /**
     * @brief      Stops the partition.
     *
     *             Disable readout buffer and triggers.
     *
     * @param[in]  aTimeout  in milliseconds
     */
    void stop( uint32_t aTimeout = 5000 ) const;


};


} // namespace pdt

#endif // __PDT_PARTITIONNODE_HPP__