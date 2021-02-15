/**
 * @file SI534xNode.hpp
 *
 * SI534xSlave and SI534xNode are classes providing an interface
 * over I2C to SI53xx PLL devices.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SI5345NODE_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SI5345NODE_HPP_

#include "pdt/SIChipSlave.hpp"
#include "pdt/I2CMasterNode.hpp"

#include "ers/ers.h"

#include <map>

namespace dunedaq {
ERS_DECLARE_ISSUE(pdt,                               ///< Namespace
                  SI534xConfigError,                 ///< Issue class name
                  " SI534xConfigError: " << message, ///< Message
                  ((std::string)message)             ///< Message parameters
)
ERS_DECLARE_ISSUE(pdt,                                       ///< Namespace
                  SI534xMissingConfigSectionError,           ///< Issue class name
                  " Missing configuration section: " << tag, ///< Message
                  ((std::string)tag)                         ///< Message parameters
)
ERS_DECLARE_ISSUE(pdt,                                                             ///< Namespace
                  SI534xRegWriteFailed,                                            ///< Issue class name
                  " Failed to write Si53xx reg: " << reg << "with data: " << data, ///< Message
                  ((std::string)reg)((std::string)data)                            ///< Message parameters
)
ERS_DECLARE_ISSUE(pdt,                                     ///< Namespace
                  SI534xRegWriteRetry,                     ///< Issue class name
                  "Retry " << attempt << " for reg " << reg,      ///< Message
                  ((std::string)attempt)((std::string)reg) ///< Message parameters
)
namespace pdt 
{

/**
 * @class      SI534xSlave
 *
 * @brief      I2C slave class to control SI5345 chips.
 * @author     Alessandro Thea
 * @date       August 2017
 */
class SI534xSlave : public SIChipSlave {
public:
    SI534xSlave(const I2CMasterNode* aMaster, uint8_t aSlaveAddress);
    virtual ~SI534xSlave();

    void configure(const std::string& aFilename) const;

    std::map<uint16_t, uint8_t> registers() const;

    std::string readConfigID() const;

private:
    typedef boost::tuple<uint16_t, uint8_t>  RegisterSetting_t;

    std::string seekHeader(std::ifstream& aFile) const;
    std::vector<RegisterSetting_t> readConfigSection( std::ifstream& aFile, std::string aTag ) const;

    void uploadConfig( const std::vector<SI534xSlave::RegisterSetting_t>& aConfig ) const;
};

/**
 * @class      SI534xNode
 *
 * @brief      uhal::Node implementing single I2C Master Slave connection to
 *             control SI5345 chips
 * @author     Alessandro Thea
 * @date       August 2013
 */
class SI534xNode : public I2CMasterNode, public SI534xSlave {
    UHAL_DERIVEDNODE(SI534xNode)
public:
    SI534xNode(const uhal::Node& aNode);
    SI534xNode(const SI534xNode& aOther);
    virtual ~SI534xNode();

};

    
} // namespace pdt
} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_SI5345NODE_HPP_ 