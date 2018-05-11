#include "pdt/SI5345Node.hpp"

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
UHAL_REGISTER_DERIVED_NODE(SI5345Node);

//-----------------------------------------------------------------------------
SI5345Slave::SI5345Slave( const I2CBaseNode* aMaster, uint8_t aAddr ) :
SIChipSlave( aMaster, aAddr ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI5345Slave::~SI5345Slave( ) {
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
void
SI5345Slave::seekHeader( std::ifstream& aFile ) const {

    std::string lLine;

    while( std::getline(aFile, lLine) ) {

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
}

//-----------------------------------------------------------------------------
// Seek Header
// Seek conf start
// read data
// Stop on conf end

std::vector<SI5345Slave::RegisterSetting_t>
SI5345Slave::readConfigSection( std::ifstream& aFile, std::string aTag ) const {

    // Line buffer
    std::string lLine;

    bool lSectionFound(false);

    std::vector<RegisterSetting_t> lConfig;
    while( std::getline(aFile, lLine) ) {


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
            PDT_LOG(kError) << "Bugger";
            throw SI5345ConfigError("Bugger");
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
SI5345Slave::configure( const std::string& aPath ) const {

    throwIfNotFile(aPath);

    std::ifstream lFile(aPath);
    std::string lLine;

    // Seek the header line first
    seekHeader(lFile);

    auto lPreamble = this->readConfigSection(lFile, "preamble");
    PDT_LOG(kDebug) << "Preamble size = " << lPreamble.size();
    auto lRegisters = this->readConfigSection(lFile, "registers");
    PDT_LOG(kDebug) << "Registers size = " << lRegisters.size();
    auto lPostAmble = this->readConfigSection(lFile, "postamble");
    PDT_LOG(kDebug) << "PostAmble size = " << lPostAmble.size();

    lFile.close();


    this->uploadConfig(lPreamble);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    this->uploadConfig(lRegisters);
    this->uploadConfig(lPostAmble);

}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void 
SI5345Slave::uploadConfig( const std::vector<SI5345Slave::RegisterSetting_t>& aConfig ) const {

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
void
SI5345Slave::configure0g( const std::string& aPath ) const {

	throwIfNotFile(aPath);

    std::ifstream lFile(aPath);

    std::string lLine;
    bool lHeaderFound(false);

	typedef boost::tuple<uint16_t, uint8_t>  RegisterSetting;

    // std::map<uint16_t, uint8_t> aConfig;
    std::vector<RegisterSetting> lConfig;
    while( std::getline(lFile, lLine) ) {

        // Is it a comment 
        if( lLine[0] == '#' ) {
            continue;
        }

        // Stop if the line is empty
        if( lLine.length() == 0 ) {
            break;
        }

        if ( lLine == "Address,Data" ) {
        	lHeaderFound = true;
            continue;
        }

        if ( !lHeaderFound ) {
        	PDT_LOG(kError) << "Bugger";
        	throw SI5345ConfigError("Bugger");
        }

        uint32_t lAddress, lData;
        char lDummy;

        std::istringstream lLineStream(lLine);
        lLineStream >> std::hex >> lAddress >> lDummy >> std::hex >> lData;

		PDT_LOG(pdt::kDebug2) << std::showbase << std::hex << "Address: " <<  lAddress << lDummy << " Data: " << lData;

		lConfig.push_back(RegisterSetting(lAddress, lData));
	}
    lFile.close();

	PDT_LOG(pdt::kInfo) << "Configuration read from file (" << lConfig.size() << " entries). Starting upload...";

	size_t k(0), lNotifyPercent(10);
	size_t lNotifyEvery(lConfig.size()/lNotifyPercent);

	for ( const auto& lSetting : lConfig ) {
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


        // PDT_LOG(kInfo) << std::showbase << std::hex 
        //                << "Reading from "  << (uint32_t)lSetting.get<0>();
        // while( lAttempt < lMaxAttempts ) {        
        //     PDT_LOG(kInfo) << "Attempt " << lAttempt;

        //     try {
        //         uint8_t lVal = this->readClockRegister(lSetting.get<0>());
        //         if (  lVal != lSetting.get<1>() ) {
        //             PDT_LOG(kError) << "-> Bugger Readback failed " << std::showbase << std::hex << lSetting.get<0>();
        //             PDT_LOG(kError) << std::showbase << std::hex 
        //                  << "   Exp " <<  (uint32_t)lSetting.get<1>() << " found " << (uint32_t)lVal;
        //             ++lAttempt;
        //             continue;
        //         }
        //     } catch( const std::exception& e) {
        //         PDT_LOG(kError) << "-> Bugger Read failed " << std::showbase << std::hex << lSetting.get<0>();
        //         PDT_LOG(kError) << "   reason: " << e.what();
        //         ++lAttempt;
        //         continue;
        //     }
        //     break;
        // }

		++k;
		if ( (k % lNotifyEvery) == 0 ) {
			PDT_LOG(kDebug) << (k/lNotifyEvery) * lNotifyPercent << "%";
		}
	}

}

//-----------------------------------------------------------------------------
std::map<uint32_t, uint32_t>
SI5345Slave::registers( ) const {
    //  boost::format fmthex("%d");
    std::map<uint32_t, uint32_t> values;

    // for( uint8_t regAddr = 0; regAddr <= 143; regAddr++ ) {
    //     if( regAddr > 11 && regAddr < 16 ) {
    //         continue;
    //     }

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

    //     values[ regAddr ] = readI2C(regAddr);
    // }

    return values;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
SI5345Node::SI5345Node( const uhal::Node& aNode ) : I2CBaseNode(aNode), SI5345Slave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI5345Node::SI5345Node( const SI5345Node& aOther ) : I2CBaseNode(aOther), SI5345Slave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI5345Node::~SI5345Node() {
}
//-----------------------------------------------------------------------------



} // namespace pdt