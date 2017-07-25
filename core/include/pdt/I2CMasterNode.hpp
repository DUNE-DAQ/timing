/**
 * @file    I2CMasterNode.hpp
 * @author  Alessandro Thea
 * @brief   Brief description
 * @date 
 */

#ifndef __PDT_I2CMASTERNODE_HPP__
#define	__PDT_I2CMASTERNODE_HPP__

// uHal Headers
#include "uhal/DerivedNode.hpp"
#include "pdt/exception.hpp"
#include "pdt/I2CBaseNode.hpp"
 
namespace pdt {

class I2CSlave;

/*!
 * @class I2CMasterNode
 * @author Alessandro Thea
 * @brief Generic class to give access to multiple I2C targets
 */
class I2CMasterNode : public I2CBaseNode {
    UHAL_DERIVEDNODE(I2CBaseNode);
public:
    I2CMasterNode(const uhal::Node& aNode );
    I2CMasterNode(const I2CMasterNode& aOther );
    virtual ~I2CMasterNode();
    
    virtual const I2CSlave&  getSlave( const std::string& name ) const;
    
private:
    void constructor();
    //! Slaves 
    boost::unordered_map<std::string,I2CSlave*> mSlaves;

};

} // namespace pdt

#endif	/* __PDT_I2CMASTERNODE_HPP__ */

