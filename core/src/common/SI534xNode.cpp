#include "pdt/SI534xNode.hpp"

// PDT headers
#include "pdt/toolbox.hpp"
#include "pdt/Logger.hpp"

#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string/predicate.hpp> 

#include <chrono>
#include <thread>

#include <fstream>
#include <sstream>

namespace pdt {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(SI534xNode);

//-----------------------------------------------------------------------------
SI534xSlave::SI534xSlave( const I2CBaseNode* aMaster, uint8_t aAddr ) :
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
    // for( std::string lLine, uint32_t lLineNum(0); std::getline(lFile, lLine); ++lLineNum) {
    // while( std::getline(aFile, lLine) ) {

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
            throw SI5345ConfigError("Incomplete file: End of file detected while seeking the header.");
        }
    }
    
    PDT_LOG(pdt::kDebug) << "Found desing ID " << lDesignID;

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
    // for( std::string lLine, uint32_t lLineNum(0); std::getline(lFile, lLine); ++lLineNum) {
    // while( std::getline(aFile, lLine) ) {

        // Is it a comment 
        if( lLine[0] == '#' ) {

            if (boost::starts_with(lLine, "# Start configuration "+aTag)) {
                lSectionFound = true;
            }

            // Section end found. Break here
            if (boost::starts_with(lLine, "# End configuration "+aTag)) {
                break;
            }

            continue;
        }

        if ( aFile.eof() ) {
            throw SI5345ConfigError("Incomplete file: End of file detected before the end of "+aTag+" section.");
        }

        // Stop if the line is empty
        if( lLine.length() == 0 ) continue;

        if ( !lSectionFound ) {
            PDT_LOG(kError) << "Main configuration section missing";
            throw SI5345ConfigError("Main configuration section missing");
        }

        uint32_t lAddress, lData;
        char lDummy;

        std::istringstream lLineStream(lLine);
        lLineStream >> std::hex >> lAddress >> lDummy >> std::hex >> lData;

        PDT_LOG(pdt::kDebug2) << std::showbase << std::hex << "Address: " <<  lAddress << lDummy << " Data: " << lData;

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

    auto lPreamble = this->readConfigSection(lFile, "preamble");
    PDT_LOG(kDebug) << "Preamble size = " << lPreamble.size();
    auto lRegisters = this->readConfigSection(lFile, "registers");
    PDT_LOG(kDebug) << "Registers size = " << lRegisters.size();
    auto lPostAmble = this->readConfigSection(lFile, "postamble");
    PDT_LOG(kDebug) << "PostAmble size = " << lPostAmble.size();

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
        throw SI5345ConfigError(lMsg.str());
    }

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void 
SI534xSlave::uploadConfig( const std::vector<SI534xSlave::RegisterSetting_t>& aConfig ) const {

    size_t k(0), lNotifyPercent(10);
    size_t lNotifyEvery = ( lNotifyPercent < aConfig.size() ? aConfig.size()/lNotifyPercent : 1);

    for ( const auto& lSetting : aConfig ) {
        PDT_LOG(kDebug) << std::showbase << std::hex 
                       << "Writing to "  << (uint32_t)lSetting.get<0>() 
                       << " data " << (uint32_t)lSetting.get<1>();

        uint32_t lMaxAttempts(2), lAttempt(0);
        while( lAttempt < lMaxAttempts ) {        
            PDT_LOG(kDebug) << "Attempt " << lAttempt;
            if ( lAttempt > 0) {
                PDT_LOG(kWarning) << "Retry " << lAttempt << " for reg " << std::showbase << std::hex <<  (uint32_t)lSetting.get<0>() ;
            }
            try {
              this->writeClockRegister(lSetting.get<0>(), lSetting.get<1>());
            } catch( const std::exception& e) {
                PDT_LOG(kError) << "-> Bugger Write failed " << std::showbase << std::hex << lSetting.get<0>();
                PDT_LOG(kError) << "   reason: " << e.what();
                ++lAttempt;
                continue;
            }
            break;
        }

        ++k;
        if ( (k % lNotifyEvery) == 0 ) {
            PDT_LOG(kDebug) << (k/lNotifyEvery) * lNotifyPercent << "%";
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
SI534xNode::SI534xNode( const uhal::Node& aNode ) : I2CBaseNode(aNode), SI534xSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI534xNode::SI534xNode( const SI534xNode& aOther ) : I2CBaseNode(aOther), SI534xSlave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI534xNode::~SI534xNode() {
}
//-----------------------------------------------------------------------------



} // namespace pdt