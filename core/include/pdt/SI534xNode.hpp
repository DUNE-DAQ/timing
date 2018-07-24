#ifndef __PDT_SI5345NODE_HPP__
#define __PDT_SI5345NODE_HPP__

#include <map>

#include "pdt/SIChipSlave.hpp"
#include "pdt/I2CBaseNode.hpp"

namespace pdt 
{

PDTExceptionClass(SI5345ConfigError, "Clock configuration error");

/**
 * @class      SI534xSlave
 *
 * @brief      I2C slave class to control SI5345 chips.
 * @author     Alessandro Thea
 * @date       August 2017
 */
class SI534xSlave : public SIChipSlave {
public:
    SI534xSlave(const I2CBaseNode* aMaster, uint8_t aSlaveAddress);
    virtual ~SI534xSlave();

    void configure(const std::string& aFilename) const;

    std::map<uint32_t, uint32_t> registers() const;

private:
    typedef boost::tuple<uint16_t, uint8_t>  RegisterSetting_t;

    void seekHeader(std::ifstream& aFile) const;
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
class SI534xNode : public I2CBaseNode, public SI534xSlave {
    UHAL_DERIVEDNODE(SI534xNode);
public:
    SI534xNode(const uhal::Node& aNode);
    SI534xNode(const SI534xNode& aOther);
    virtual ~SI534xNode();

};

    
} // namespace pdt

#endif /* __PDT_SI5345NODE_HPP__ */