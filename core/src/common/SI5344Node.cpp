#include "pdt/SI5344Node.hpp"

// PDT headers
#include "pdt/toolbox.hpp"
#include "pdt/Logger.hpp"

#include <boost/tuple/tuple.hpp>

#include <fstream>
#include <sstream>

namespace pdt {

// uHAL Node registation
UHAL_REGISTER_DERIVED_NODE(SI5344Node);

//-----------------------------------------------------------------------------
SI5344Slave::SI5344Slave( const I2CBaseNode* aMaster, uint8_t aAddr ) :
I2CSlave( aMaster, aAddr ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI5344Slave::~SI5344Slave( ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
SI5344Slave::readPage( ) const {

    PDT_LOG(kNotice) << "<- Reading page ";

	// Read from the page address (0x1?)
	return readI2C(0x1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SI5344Slave::switchPage( uint8_t aPage ) const {

	// Prepare a data block with address and new page
	// std::vector<uint8_t> lData = {0x1, aPage};
    PDT_LOG(kNotice) << "-> Switching to page " 
        << std::showbase
        << std:: hex
        << (uint32_t) aPage;
	writeI2C(0x1, aPage);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
SI5344Slave::readDeviceVersion( ) const {

	// Go to the right page
	switchPage(0x0);
	// Read 2 words from 0x2
	auto lVersion = readI2CArray(0x2, 2);
	
	return (((uint32_t)lVersion[1] << 8) + (uint32_t)lVersion[0]);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint8_t
SI5344Slave::readClockRegister( uint16_t aAddr ) const {

	uint8_t lRegAddr = (aAddr & 0xff);
	uint8_t lPageAddr = (aAddr >> 8) & 0xff;
    PDT_LOG(kWarning) << std::showbase << std::hex 
        << "Read Address " << (uint32_t)aAddr 
        << " reg: " << (uint32_t)lRegAddr 
        << " page: " << (uint32_t)lPageAddr;
	// Change page only when required.
	// (The SI5344 don't like to have the page register id to be written all the time.)
	uint8_t lCurrentPage = readPage();
    // PDT_LOG(kWarning) << std::showbase << std::hex 
        // << "current page " << (uint32_t)lCurrentPage 
        // << " dest page " << (uint32_t)lPageAddr;
	if ( lPageAddr != lCurrentPage ) {
		switchPage(lPageAddr);
	}

	// Read the register
	return readI2C( lRegAddr );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SI5344Slave::writeClockRegister( uint16_t aAddr, uint8_t aData ) const {

	uint8_t lRegAddr = (aAddr & 0xff);
	uint8_t lPageAddr = (aAddr >> 8) & 0xff;

    PDT_LOG(kWarning) << std::showbase << std::hex 
        << "Write Address " << (uint32_t)aAddr 
        << " reg: " << (uint32_t)lRegAddr 
        << " page: " << (uint32_t)lPageAddr;
	// Change page only when required.
	// (The SI5344 don't like to have the page register id to be written all the time.)
	uint8_t lCurrentPage = readPage();
	if ( lPageAddr != lCurrentPage ) {
		switchPage(lPageAddr);
	}

	return writeI2C( lRegAddr, aData );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
SI5344Slave::configure( const std::string& aPath ) const {

	throwIfNotFile(aPath);

    std::ifstream lFile(aPath);

    std::string lLine;
    bool lHeaderFound(false);

	typedef boost::tuple<uint16_t, uint8_t>  RegisterSetting;

    // std::map<uint16_t, uint8_t> lConfig;
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
        	throw SI5344ConfigError("Bugger");
        }

        uint32_t lAddress, lData;
        char lDummy;

        std::istringstream lLineStream(lLine);
        lLineStream >> std::hex >> lAddress >> lDummy >> std::hex >> lData;

		PDT_LOG(pdt::kDebug2) << std::showbase << std::hex << "Address: " <<  lAddress << lDummy << " Data: " << lData;

		lConfig.push_back(RegisterSetting(lAddress, lData));
	}
	PDT_LOG(pdt::kInfo) << "Configuration read from file (" << lConfig.size() << " entries). Starting upload...";

	size_t k(0), lNotifyPercent(10);
	size_t lNotifyEvery(lConfig.size()/lNotifyPercent);

	for ( const auto& lSetting : lConfig ) {
		PDT_LOG(kInfo) << std::showbase << std::hex 
                       << "Writing to "  << (uint32_t)lSetting.get<0>() 
                       << " data " << (uint32_t)lSetting.get<1>();

        uint32_t lMaxAttempts(2), lAttempt(0);
        while( lAttempt < lMaxAttempts ) {        
            PDT_LOG(kInfo) << "Attempt " << lAttempt;
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

	lFile.close();
}

//-----------------------------------------------------------------------------
std::map<uint32_t, uint32_t>
SI5344Slave::registers( ) const {
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
SI5344Node::SI5344Node( const uhal::Node& aNode ) : I2CBaseNode(aNode), SI5344Slave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI5344Node::SI5344Node( const SI5344Node& aOther ) : I2CBaseNode(aOther), SI5344Slave(this, this->getSlaveAddress("i2caddr") ) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
SI5344Node::~SI5344Node() {
}
//-----------------------------------------------------------------------------



} // namespace pdt