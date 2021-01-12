#ifndef __PDT_ENDPOINTNODE_HPP__
#define __PDT_ENDPOINTNODE_HPP__

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "pdt/exception.hpp"
#include "pdt/FrequencyCounterNode.hpp"

namespace pdt {

/**
 * @brief      Base class for timing IO nodes.
 */
class EndpointNode : public TimingNode {
    UHAL_DERIVEDNODE(EndpointNode)
public:
    EndpointNode(const uhal::Node& aNode);
    ~EndpointNode();

    /**
     * @brief     Print the status of the timing node.
     */
    std::string getStatus(bool aPrint=false) const override;

    /**
     * @brief      Enable the endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void enable(uint32_t partition=0, uint32_t address=0) const;

    /**
     * @brief      Disable the endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void disable() const;

    /**
     * @brief      Reset the endpoint
     *
     * @return     { description_of_the_return_value }
     */
    void reset(uint32_t partition=0, uint32_t address=0) const;

    /**
     * @brief      Read the current timestamp word.
     *
     * @return     { description_of_the_return_value }
     */
    uint64_t readTimestamp() const;

    /**
     * @brief      Read the number of words in the data buffer.
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t readBufferCount() const;
    
    /**
     * @brief      Read the contents of the endpoint data buffer.
     *
     * @return     { description_of_the_return_value }
     */
    uhal::ValVector< uint32_t > readDataBuffer(bool aReadall=false) const;

    /**
     * @brief      Print the contents of the endpoint data buffer.
     *
     * @return     { description_of_the_return_value }
     */
    std::string getDataBufferTable(bool aReadall=false, bool aPrint=false) const;

    /**
     * @brief      Read the endpoint clock frequency.
     *
     * @return     { description_of_the_return_value }
     */
    double readClockFrequency() const;

    /**
     * @brief      Read the endpoint wrapper version
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t readVersion() const;
};


} // namespace pdt

#endif // __PDT_ENDPOINTNODE_HPP__