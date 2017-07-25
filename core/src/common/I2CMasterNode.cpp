/* 
 * File:   OpenCoresI2CMasterNode.cpp
 * Author: ale
 * 
 * Created on August 29, 2014, 4:47 PM
 */

#include "pdt/I2CMasterNode.hpp"


#include <boost/lexical_cast.hpp>
#include <boost/unordered/unordered_map.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

#include "pdt/exception.hpp"
#include "pdt/Logger.hpp"
#include "pdt/toolbox.hpp"
#include "pdt/I2CSlave.hpp"

namespace pdt {

UHAL_REGISTER_DERIVED_NODE(I2CMasterNode);

//____________________________________________________________________________//
I2CMasterNode::I2CMasterNode( const uhal::Node& aNode ) : I2CBaseNode(aNode) {
    constructor();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CMasterNode::I2CMasterNode(const I2CMasterNode& aOther ) : I2CBaseNode(aOther) {
    constructor();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void I2CMasterNode::constructor() {
   
    /// Use the list of addresses to build plain slaves
    boost::unordered_map<std::string, uint8_t>::const_iterator lIt;
    for( lIt = mSlavesAddresses.begin(); lIt != mSlavesAddresses.end(); ++lIt) {
        mSlaves.insert(std::make_pair( lIt->first, new I2CSlave( this, lIt->second ) ) );
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
I2CMasterNode::~I2CMasterNode() {
    boost::unordered_map<std::string, I2CSlave*>::iterator lIt;
    for ( lIt = mSlaves.begin(); lIt != mSlaves.end(); ++lIt ) {
        // Delete slaves
        delete lIt->second;
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
const I2CSlave&
I2CMasterNode::getSlave(const std::string& name) const {
    boost::unordered_map<std::string, I2CSlave*>::const_iterator lIt = mSlaves.find(name);
    if ( lIt == mSlaves.end() ) {
        pdt::I2CSlaveNotFound lExc(std::string("Slave ")+name+" not found.");
        PDT_LOG(kError) << lExc.what();
        throw lExc;
    }
    return *(lIt->second);
}
//-----------------------------------------------------------------------------

} // namespace pdt

