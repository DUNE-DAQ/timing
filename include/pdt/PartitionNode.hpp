/**
 * @file PartitionNode.hpp
 *
 * PartitionNode is a class providing an interface
 * to the master partition firmware block.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PARTITIONNODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PARTITIONNODE_HPP_

// PDT Headers
#include "TimingIssues.hpp"
#include "pdt/TimingNode.hpp"
#include "pdt/timingmon/Structs.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>

namespace dunedaq {
ERS_DECLARE_ISSUE(pdt,                                                                          
                  EventReadError,                                                                               
                  " Failed to read events from partition buffer. Requested: " << std::to_string(red_number_events) 
                                                      << " Available: " << std::to_string(available_number_events), 
                  ((uint)red_number_events)((uint)available_number_events)                                                                                                      
)

ERS_DECLARE_ISSUE(pdt,                                            
                  RunRequestTimeoutExpired,
                  " Failed to start after " << timeout << " milliseconds",
                  ((uint)timeout)
)

namespace pdt {

struct PartitionCounts {
    std::vector<uint32_t> accepted;
    std::vector<uint32_t> rejected;
};

/**
 * @brief      Class for partition node.
 */
class PartitionNode : public TimingNode {
    UHAL_DERIVEDNODE(PartitionNode)
public:
    PartitionNode(const uhal::Node& aNode);
    virtual ~PartitionNode();

    static const uint32_t kWordsPerEvent;

    /**
     * @brief      Reads a command mask.
     *
     * @return     The current value of the command mask.
     */
    uint32_t read_trigger_mask() const;


    /**
     * @brief      Read the number words in buffer.
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t read_buffer_word_count() const;

    /**
     * @brief      Count the number of events available in the readout buffer
     *
     * @return     { description_of_the_return_value }
     */
    uint32_t num_events_in_buffer() const;
    
    /**
     * @brief      Reads a rob warning overflow.
     *
     * @return     { description_of_the_return_value }
     */
    bool read_rob_warning_overflow() const;

    /**
     * @brief      Reads a rob error.
     *
     * @return     { description_of_the_return_value }
     */
    bool read_rob_error() const;
    
    /**
     * @brief      Read multiple events from the rob.
     *
     * @param[in]  aNumEvents  Number of events to read
     *
     * @return     Standard vector containing all events extracted from rob
     */
    std::vector<uint32_t> read_events( size_t aNumEvents = 0 ) const;


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
    // void writeTriggerMask( uint32_t aMask ) const;

    void configure( uint32_t aTrigMask, bool aEnableSpillGate, bool aRateCtrl=0x1 ) const;
    
    /**
     * @brief      Writes the trigger mask.
     *
     * @param[in]  aMask  A mask
     */
    void configure_rate_ctrl( bool aRateCtrl=0x1 ) const;

    /**
     * @brief      Enables the triggers.
     *
     * @param[in]  aEnable  The enable switch
     */
    void enable_triggers( bool aEnable = true ) const;

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


    /**
     * @brief      Reads command counts.
     *
     * @return     { description_of_the_return_value }
     */
    PartitionCounts read_command_counts() const;

    /**
     * @brief     Print the status of the timing node.
     */
    std::string get_status(bool aPrint=false) const override;

    /**
     * @brief     Fill the partition monitoring structure.
     */
    void get_info(timingmon::TimingPartitionMonitorData& mon_data) const;

};


} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_PARTITIONNODE_HPP_