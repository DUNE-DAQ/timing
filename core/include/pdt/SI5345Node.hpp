#ifndef __PDT_SI5345NODE_HPP__
#define __PDT_SI5345NODE_HPP__

#include <map>

#include "pdt/SIChipSlave.hpp"
#include "pdt/I2CBaseNode.hpp"

namespace pdt 
{

PDTExceptionClass(SI5345ConfigError, "Clock configuration error");

/**
 * @class      SI5345Slave
 *
 * @brief      I2C slave class to control SI5345 chips.
 * @author     Alessandro Thea
 * @date       August 2017
 */
class SI5345Slave : public SIChipSlave {
public:
    SI5345Slave(const I2CBaseNode* aMaster, uint8_t aSlaveAddress);
    virtual ~SI5345Slave();

    void configure(const std::string& aFilename) const;

    std::map<uint32_t, uint32_t> registers() const;

private:
    typedef boost::tuple<uint16_t, uint8_t>  RegisterSetting_t;

    void seekHeader(std::ifstream& aFile) const;
    std::vector<RegisterSetting_t> readConfigSection( std::ifstream& aFile, std::string aTag ) const;

    void uploadConfig( const std::vector<SI5345Slave::RegisterSetting_t>& aConfig ) const;
};

/**
 * @class      SI5345Node
 *
 * @brief      uhal::Node implementing single I2C Master Slave connection to
 *             control SI5345 chips
 * @author     Alessandro Thea
 * @date       August 2013
 */
class SI5345Node : public I2CBaseNode, public SI5345Slave {
    UHAL_DERIVEDNODE(SI5345Node);
public:
    SI5345Node(const uhal::Node& aNode);
    SI5345Node(const SI5345Node& aOther);
    virtual ~SI5345Node();

};

    
} // namespace pdt

#endif /* __PDT_SI5345NODE_HPP__ */