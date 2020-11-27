/*
 * File:   exception.h
 * Author: ale
 *
 * Created on August 21, 2013, 2:29 PM
 */

#ifndef __PDT_EXCEPTION_HPP__
#define	__PDT_EXCEPTION_HPP__


#include <exception>
#include <string>
#include <vector>

#define PDTExceptionImpl(ClassName, ClassDescription)\
public:\
  virtual ~ClassName() throw () {}\
\
  std::string description() const { return std::string(ClassDescription); }


#define PDTExceptionClass(ClassName, ClassDescription)\
class ClassName : public pdt::exception {\
public:\
  ClassName() : pdt::exception() {}\
  ClassName(const std::string& aString) : pdt::exception(aString) {}\
  PDTExceptionImpl(ClassName, ClassDescription)\
};


namespace pdt {

///! MP7 base exception class

class exception : public std::exception {
public:
    exception();
    exception(const std::string& aExc);
    virtual ~exception() throw ();

    const char* what() const throw ();

    void append(const std::string& aExc);

    virtual std::string description() const = 0;

private:
    std::string mInfo;
};


PDTExceptionClass(MP7HelperException, "Exception class to handle MP7 specific exceptions");

// Generic exceptions
// PDTExceptionClass(ArgumentError,"Exception class to handle argument errors")
PDTExceptionClass(WrongFileExtension, "File has the wrong file-extension for the class trying to open it");
PDTExceptionClass(FileNotFound, "File was not found");
PDTExceptionClass(CorruptedFile, "File was corrupted");
PDTExceptionClass(InvalidExtension, "Invalid extension");
PDTExceptionClass(InvalidConfigFile, "Exception class to handle invalid configuration files.")
PDTExceptionClass(UnmatchedRequirement, "Exception class to handle invalid unmatched configuration requirements.")
PDTExceptionClass(EntryNotFoundError, "Entry not found");


PDTExceptionClass(I2CSlaveNotFound, "Exception class to handle missing I2C slaves");
PDTExceptionClass(I2CException, "Exception class to handle I2C Exceptions");

PDTExceptionClass(UnknownBoardType, "Unknown board type");
PDTExceptionClass(UnknownCarrierType, "Unknown carrier type");
PDTExceptionClass(UnknownDesignType, "Unknown design type");
PDTExceptionClass(UnknownBoardUID, "Unknown board UID");

PDTExceptionClass(MissingBoardTypeMapEntry, "Mising board type map entry");
PDTExceptionClass(MissingBoardRevisionMapEntry, "Mising board revision map entry");
PDTExceptionClass(MissingCarrierTypeMapEntry, "Mising carrier type map entry");
PDTExceptionClass(MissingDesignTypeMapEntry, "Mising design type map entry");

PDTExceptionClass(ClockConfigNotFound, "Clock config file not found");

PDTExceptionClass(InvalidSFPId, "SFP id out of range for SFP buses list");

PDTExceptionClass(InvalidDACId, "DAC id out of range for DAC devices list");

PDTExceptionClass(EchoTimeout, "Echo timeout");

PDTExceptionClass(FormatCountersTableNodesTitlesMismatch, "Mismatch between number counters nodes and titles");

PDTExceptionClass(SFPDDMI2CAddressSwapUnsupported, "SFP DDM I2C address swap unsupported");

PDTExceptionClass(SFPDDMUnsupported, "SFP does not support DDM");

PDTExceptionClass(SoftTxLaserControlUnsupported, "SFP does not support soft tx laser control");

PDTExceptionClass(SFPUnreachable, "Failed to reach SFP");

PDTExceptionClass(UnsupportedFunction, "Requested function unsupported");

PDTExceptionClass(BadRequestedFakeTriggerRate, "Requested trigger rate out of range");

PDTExceptionClass(UpstreamEndpointFailedToLock, "Upstream endpoint failed to lock");


}


#endif	/* __PDT_EXCEPTION_HPP__ */


