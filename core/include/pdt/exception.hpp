/*
 * File:   exception.h
 * Author: ale
 *
 * Created on August 21, 2013, 2:29 PM
 */

#ifndef __MP7_EXCEPTION_HPP__
#define	__MP7_EXCEPTION_HPP__


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

    
// PDTExceptionClass(IdentificationError, "MP7 board model cannot be detected");

// // MMC Exceptions
// PDTExceptionClass(SDCardError, " Exception class to handle errors related to SDCard access");

// // Ctrl Exceptions
// PDTExceptionClass(RegionKindError, "Exception class to handle region kind readout errors");
// PDTExceptionClass(Clock40NotInReset, "Exception class to handle cases where the clock source is change without keeping the line in reset");
// PDTExceptionClass(Clock40LockFailed, "Exception class to handle failure of clock 40 locking");
// PDTExceptionClass(XpointConfigTimeout, "Exception class to handle Xpoint configuration timeout");
// PDTExceptionClass(SI5326ConfigurationTimeout, "Exception class to handle si5326 configuration timeout");

// // TTC Exceptions
// PDTExceptionClass(BC0LockFailed, "Exception class to handle failure of BC0 lock");
// PDTExceptionClass(TTCFrequencyInvalid, "Exception class to handle TTC invalid frequency readings");
// PDTExceptionClass(TTCPhaseError, "Exception class to handle TTC phase errors");

// // Alingment Exceptions
// PDTExceptionClass(AlignmentTimeout, "Exception class to handle alignment timeout errors");
// PDTExceptionClass(AlignmentShiftFailed, "Failure to apply alignment shift");
// PDTExceptionClass(AlignmentErrorsDetected, "Exception class to handle alignment errors");
// // PDTExceptionClass(AlignmentError, "Exception class to handle alignment errors");


// PDTExceptionClass(FormatterError, "Exception class to handle formatter errors");
// PDTExceptionClass(LinkError, "Exception class to handle link errors (e.g. alignment/CRC errors)")
// PDTExceptionClass(CaptureFailed, "Capture operation failed")

// // MGTs Exceptions
// PDTExceptionClass(MGTFSMResetTimeout, "Exception class to handle failure to reset the transciever's FSMs");
// PDTExceptionClass(MGTChannelIdOutOfBounds, "Exception class to out of bounds channel ids");

// PDTExceptionClass(BufferLockFailed, "Exception class to handle failure of Buffer lock");
// PDTExceptionClass(BufferConfigError, "Exception class to handle buffer misconfiguration");
// PDTExceptionClass(BufferSizeExceeded, "Exception class to handle buffer size errors");

PDTExceptionClass(I2CSlaveNotFound, "Exception class to handle missing I2C slaves");
PDTExceptionClass(I2CException, "Exception class to handle I2C Exceptions");
// PDTExceptionClass(MinipodChannelNotFound, "Requested MiniPOD channel does not exist");

// // Readout exceptions
// PDTExceptionClass(ZeroSuppressionNotAvailable, "Zero Suppression module not implemented");
// PDTExceptionClass(InvalidCaptureId, "Invalid capture id");
// PDTExceptionClass(CaptureNumberMismatch, "Expected number of capture ides doesn't match");
// PDTExceptionClass(MaskSizeError, "Zero suppression Mask size error");
// PDTExceptionClass(ReadoutMenuInconsistentWithFirmware, "Mismatch between readout menu and firmware error");
// PDTExceptionClass(ReadoutMenuConsistencyCheckFailed, "Mismatch between readout menu and firmware error");

        
}


#endif	/* __MP7_EXCEPTION_HPP__ */


