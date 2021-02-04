#include "pdt/SI534xNode.hpp"

// PDT headers
#include "pdt/toolbox.hpp"
#include "ers/ers.h"

#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string/predicate.hpp> 

#include <chrono>
#include <thread>

#include <fstream>
#include <sstream>

namespace pdt {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(SI534xNode)

//-----------------------------------------------------------------------------
SI534xSlave::SI534xSlave( const I2CMasterNode* aMaster, uint8_t aAddr ) :
SIChipSlave( aMaster, aAddr ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI534xSlave::~SI534xSlave( ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SI534xSlave::readConfigID() const {
    std::string id;

    for ( size_t i(0); i<8; ++i) {
        id += (char)readClockRegister(0x26b+i);
    }
    return id;

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
SI534xSlave::seekHeader( std::ifstream& aFile ) const {

    // std::string lLine;
    std::string lDesignID;

    std::string lLine;
    uint32_t lLineNum;
    for( lLineNum = 1; std::getline(aFile, lLine); ++lLineNum) {

        // Gracefully deal with those damn dos-encoded files
        if ( lLine.back() == '\r' )
            lLine.pop_back();

        // Section end found. Break here
        if (boost::starts_with(lLine, "# Design ID:")) {
            lDesignID = lLine.substr(13);
        }

        // Skip comments
        if( lLine[0] == '#' ) continue;

        // Stop if the line is empty
        if( lLine.length() == 0 )  continue;

        // OK, header found, stop here
        if ( lLine == "Address,Data" ) break;

        if ( aFile.eof() ) {
            throw SI534xConfigError(ERS_HERE, "SI534xSlave", "Incomplete file: End of file detected while seeking the header.");
        }
    }
    
    ERS_DEBUG(0, "Found desing ID " << lDesignID);

    return lDesignID;
}

//-----------------------------------------------------------------------------
// Seek Header
// Seek conf start
// read data
// Stop on conf end

std::vector<SI534xSlave::RegisterSetting_t>
SI534xSlave::readConfigSection( std::ifstream& aFile, std::string aTag ) const {

    // Line buffer
    // std::string lLine;

    bool lSectionFound(false);

    std::vector<RegisterSetting_t> lConfig;
    std::string lLine;
    uint32_t lLineNum;
    for( lLineNum = 1; std::getline(aFile, lLine); ++lLineNum) {

        // Gracefully deal with those damn dos-encoded files
        if ( lLine.back() == '\r' )
            lLine.pop_back();

        // Is it a comment 
        if( lLine[0] == '#' ) {

            if ( aTag.empty() ) continue;

            if (boost::starts_with(lLine, "# Start configuration "+aTag)) {
                lSectionFound = true;
            }

            // Section end found. Break here
            if (boost::starts_with(lLine, "# End configuration "+aTag)) {
                break;
            }

            continue;
        }

        // Oops
        if ( aFile.eof() ) {
            if ( aTag.empty() )
                return lConfig;
            else
                throw SI534xConfigError(ERS_HERE, "SI534xSlave", "Incomplete file: End of file detected before the end of "+aTag+" section.");
        }

        // Stop if the line is empty
        if( lLine.length() == 0 ) continue;

        // If no sec
        if ( !lSectionFound and !aTag.empty()) {
            throw SI534xMissingConfigSectionError(ERS_HERE, "SI534xSlave", aTag);
        }

        uint32_t lAddress, lData;
        char lDummy;

        std::istringstream lLineStream(lLine);
        lLineStream >> std::hex >> lAddress >> lDummy >> std::hex >> lData;

        std::stringstream debug_stream;
        debug_stream << std::showbase << std::hex << "Address: " <<  lAddress << lDummy << " Data: " << lData;
        ERS_DEBUG(2, debug_stream.str());

        lConfig.push_back(RegisterSetting_t(lAddress, lData));
    }

    return lConfig;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
SI534xSlave::configure( const std::string& aPath ) const {

    throwIfNotFile(aPath);

    std::ifstream lFile(aPath);
    std::string lLine;
    std::string lConfDesignID;

    // Seek the header line first
    lConfDesignID = seekHeader(lFile);
    std::ifstream::pos_type lHdrEnd = lFile.tellg();

    // auto lPreamble = this->readConfigSection(lFile, "preamble");
    // PDT_LOG(kDebug) << "Preamble size = " << lPreamble.size();
    // auto lRegisters = this->readConfigSection(lFile, "registers");
    // PDT_LOG(kDebug) << "Registers size = " << lRegisters.size();
    // auto lPostAmble = this->readConfigSection(lFile, "postamble");
    // PDT_LOG(kDebug) << "PostAmble size = " << lPostAmble.size();


    std::vector<SI534xSlave::RegisterSetting_t> lPreamble, lRegisters, lPostAmble; 
    
    try {
        lPreamble = this->readConfigSection(lFile, "preamble");
        lRegisters = this->readConfigSection(lFile, "registers");
        lPostAmble = this->readConfigSection(lFile, "postamble");
    } catch ( SI534xMissingConfigSectionError& ) {
        lFile.seekg(lHdrEnd);
        lPreamble.clear();
        lRegisters = this->readConfigSection(lFile, "");
        lPostAmble.clear();
    }

    ERS_DEBUG(0, "Preamble size = " << lPreamble.size());
    ERS_DEBUG(0, "Registers size = " << lRegisters.size());
    ERS_DEBUG(0, "PostAmble size = " << lPostAmble.size());

    lFile.close();

    try {
        this->writeClockRegister(0x1E, 0x2);
    } catch ( pdt::I2CException& lExc ) {
        // Do nothing.
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    this->uploadConfig(lPreamble);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    this->uploadConfig(lRegisters);
    this->uploadConfig(lPostAmble);

    std::string lChipDesignID = this->readConfigID();

    if ( lConfDesignID != lChipDesignID ) {
        std::ostringstream lMsg;
        lMsg << "Post-configuration check failed: Loaded design ID " << lChipDesignID << " does not match the configurationd design id " << lConfDesignID << std::endl;
        throw SI534xConfigError(ERS_HERE, "SI534xSlave", lMsg.str());
    }

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void 
SI534xSlave::uploadConfig( const std::vector<SI534xSlave::RegisterSetting_t>& aConfig ) const {

    size_t k(0), lNotifyPercent(10);
    size_t lNotifyEvery = ( lNotifyPercent < aConfig.size() ? aConfig.size()/lNotifyPercent : 1);

    for ( const auto& lSetting : aConfig ) {
        std::stringstream debug_stream;
        debug_stream   << std::showbase << std::hex 
                       << "Writing to "  << (uint32_t)lSetting.get<0>() 
                       << " data " << (uint32_t)lSetting.get<1>();
        ERS_DEBUG(0, debug_stream.str());

        uint32_t lMaxAttempts(2), lAttempt(0);
        while( lAttempt < lMaxAttempts ) {        
            ERS_DEBUG(0, "Attempt " << lAttempt);
            if ( lAttempt > 0) {
                ers::warning(SI534xRegWriteRetry(ERS_HERE, "SI534xSlave", formatRegValue((uint32_t)lSetting.get<0>()) ));
            }
            try {
              this->writeClockRegister(lSetting.get<0>(), lSetting.get<1>());
            } catch( const std::exception& e) {
                ers::error(SI534xRegWriteFailed(ERS_HERE, "SI534xSlave", formatRegValue((uint32_t)lSetting.get<0>()), formatRegValue((uint32_t)lSetting.get<1>()), e));
                ++lAttempt;
                continue;
            }
            break;
        }

        ++k;
        if ( (k % lNotifyEvery) == 0 ) {
            ERS_DEBUG(0, (k/lNotifyEvery) * lNotifyPercent << "%");
        }
    }

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::map<uint16_t, uint8_t>
SI534xSlave::registers( ) const {
    //  boost::format fmthex("%d");
    std::map<uint16_t, uint8_t> values;

    for( uint8_t regAddr = 0xc; regAddr <= 0x12; regAddr++ ) {
        if( regAddr > 0xf && regAddr < 0x11 ) {
            continue;
        }

    //     if( regAddr > 25 && regAddr < 31 ) {
    //         continue;
    //     }

    //     if( regAddr > 36 && regAddr < 40 ) {
    //         continue;
    //     }

    //     if( regAddr > 48 && regAddr < 55 ) {
    //         continue;
    //     }

    //     if( regAddr > 55 && regAddr < 128 ) {
    //         continue;
    //     }

    //     if( regAddr == 133 ) {
    //         continue;
    //     }

    //     if( regAddr == 137 ) {
    //         continue;
    //     }

    //     if( regAddr > 139 && regAddr < 142 ) {
    //         continue;
    //     }

        values[ regAddr ] = readClockRegister(regAddr);
    }

    return values;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SI534xNode::SI534xNode( const uhal::Node& aNode ) : I2CMasterNode(aNode), SI534xSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI534xNode::SI534xNode( const SI534xNode& aOther ) : I2CMasterNode(aOther), SI534xSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI534xNode::~SI534xNode() {
}
//-----------------------------------------------------------------------------



} // namespace pdt