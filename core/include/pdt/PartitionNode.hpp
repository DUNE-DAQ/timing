#ifndef __PDT_PARTITIONNODE_HPP__
#define __PDT_PARTITIONNODE_HPP__

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/exception.hpp"

namespace pdt {


PDTExceptionClass(EventReadError, "Error while reading events from the partition");

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
    uint32_t readCommandMask() const;


    /**
     * @brief      Reads the number words in buffer.
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t readBufferWordCount() const;

    /**
     * @brief      { function_description }
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t numEventsInBuffer() const;
    
    /**
     * @brief      Reads an event.
     *
     * @return     { description_of_the_return_value }
     */
    std::vector<uint32_t> readEvents( size_t aNumEvents = 0 ) const;

    /**
     * @brief      Enables the partition now.
     *
     * @param[in]  aEnable  A enable
     */
    void enable( bool aEnable=true, bool aDispatch=true ) const;

    /**
     * @brief      Sets the command mask.
     *
     * @param[in]  aMask  A mask
     */
    void setCommandMask( uint32_t aMask ) const;

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
     */
    void start() const;

    /**
     * @brief      Stops the partition.
     *
     *             Disable readout buffer and triggers.
     */
    void stop() const;


};

} // namespace pdt

#endif // __PDT_PARTITIONNODE_HPP__