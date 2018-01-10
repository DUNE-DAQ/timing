#ifndef __PDT_SI5344_HPP__
#define __PDT_SI5344_HPP__

// uHal Headers
#include "uhal/DerivedNode.hpp"

namespace pdt {

/**
 * @brief      Class for partition node.
 */
class PartitionNode : public uhal::Node {
    UHAL_DERIVEDNODE(PartitionNode);
public:
    PartitionNode(const uhal::Node& aNode);
    virtual ~PartitionNode();

    /**
     * @brief      Reads a command mask.
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t readCommandMask() const;

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

#endif