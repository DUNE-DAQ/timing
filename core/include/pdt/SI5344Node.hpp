#ifndef __PDT_SI5344NODE_HPP__
#define __PDT_SI5344NODE_HPP__

#include <map>

#include "pdt/SIChipSlave.hpp"
#include "pdt/I2CBaseNode.hpp"

namespace pdt 
{

PDTExceptionClass(SI5344ConfigError, "Clock configuration error");

/**
 * @class      SI5344Slave
 *
 * @brief      I2C slave class to control SI5344 chips.
 * @author     Alessandro Thea
 * @date       August 2017
 */
class SI5344Slave : public SIChipSlave {
public:
    SI5344Slave(const I2CBaseNode* aMaster, uint8_t aSlaveAddress);
    virtual ~SI5344Slave();


    void configure(const std::string& aFilename) const;
    
    std::string readConfigID() const;

    std::map<uint32_t, uint32_t> registers() const;

};

/**
 * @class      SI5344Node
 *
 * @brief      uhal::Node implementing single I2C Master Slave connection to
 *             control SI5344 chips
 * @author     Alessandro Thea
 * @date       August 2013
 */
class SI5344Node : public I2CBaseNode, public SI5344Slave {
    UHAL_DERIVEDNODE(SI5344Node);
public:
    SI5344Node(const uhal::Node& aNode);
    SI5344Node(const SI5344Node& aOther);
    virtual ~SI5344Node();

};

	
} // namespace pdt

#endif /* __PDT_SI5344NODE_HPP__ */