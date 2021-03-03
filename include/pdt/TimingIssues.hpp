/*
 * File:   exception.h
 * Author: ale
 *
 * Created on August 21, 2013, 2:29 PM
 */

#ifndef TIMING_BOARD_SOFTWARE_SRC_TIMING_ISSUES_HPP_
#define	TIMING_BOARD_SOFTWARE_SRC_TIMING_ISSUES_HPP_

#include "ers/Issue.hpp"

#include <string>

namespace dunedaq {

//PDTExceptionClass(MP7HelperException, "Exception class to handle MP7 specific exceptions")

// Generic exceptions
ERS_DECLARE_ISSUE(pdt,                             ///< Namespace
                  FileNotFound,                    ///< Issue class name
                  file_path << " does not exist!", ///< Message
                  ((std::string)file_path)         ///< Message parameters
)
ERS_DECLARE_ISSUE(pdt,                       ///< Namespace
                  CorruptedFile,             ///< Issue class name
                  file_path << " corrupted", ///< Message
                  ((std::string)file_path)   ///< Message parameters
)
ERS_DECLARE_ISSUE_BASE(pdt,                                                             ///< Namespace
                       FileIsDirectory,                                                 ///< Issue class name
                       CorruptedFile,                                                   ///< Base class of the issue
                       file_path << " is a directory", ///< Log Message from the issue
                       ((std::string)file_path),                                           ///< Base class attributes
                       ERS_EMPTY                                                        ///< Attribute of this class
)

// I2C related issues
ERS_DECLARE_ISSUE(pdt,                                ///< Namespace
                  I2CException,                       ///< Issue class name
                  " I2CException on bus: " << bus_id, ///< Message
                  ((std::string)bus_id)               ///< Message parameters
)

ERS_DECLARE_ISSUE_BASE(pdt,                                                             ///< Namespace
                       I2CDeviceNotFound,                                               ///< Issue class name
                       I2CException,                                                    ///< Base class of the issue
                       " I2C device: " << device_id << " not found on bus: " << bus_id, ///< Log Message from the issue
                       ((std::string)bus_id),                                           ///< Base class attributes
                       ((std::string)device_id)                                         ///< Attribute of this class
)

ERS_DECLARE_ISSUE_BASE(pdt,                                                                            ///< Namespace
                       I2CBusArbitrationLost,                                                          ///< Issue class name
                       I2CException,                                                                   ///< Base class of the issue
                       " I2C bus: " << bus_id << " arbitration lost. Is another application running?", ///< Log Message from the issue
                       ((std::string)bus_id),                                                          ///< Base class attributes
                       ERS_EMPTY                                                                       ///< Attribute of this class
)

ERS_DECLARE_ISSUE_BASE(pdt,                                                                                                                 ///< Namespace
                       I2CTransactionTimeout,                                                                                               ///< Issue class name
                       I2CException,                                                                                                        ///< Base class of the issue
                       " I2C bus: " << bus_id << " error. Transaction timeout - the 'Transfer in Progress' bit remained high for too long", ///< Log Message from the issue
                       ((std::string)bus_id),                                                                                               ///< Base class attributes
                       ERS_EMPTY                                                                                                            ///< Attribute of this class
)


ERS_DECLARE_ISSUE_BASE(pdt,                                                   ///< Namespace
                       I2CNoAcknowledgeReceived,                              ///< Issue class name
                       I2CException,                                          ///< Base class of the issue
                       " I2C bus: " << bus_id << " error. No acknowledge received", ///< Log Message from the issue
                       ((std::string)bus_id),                                 ///< Base class attributes
                       ERS_EMPTY                                              ///< Attribute of this class
)

ERS_DECLARE_ISSUE_BASE(pdt,                                                                      ///< Namespace
                       I2CTransferFinishedBusStillBusy,                                          ///< Issue class name
                       I2CException,                                                             ///< Base class of the issue
                       " I2C bus: " << bus_id << " error. Transfer finished but bus still busy", ///< Log Message from the issue
                       ((std::string)bus_id),                                 ///< Base class attributes
                       ERS_EMPTY                                              ///< Attribute of this class
)

ERS_DECLARE_ISSUE(pdt,                                   ///< Namespace
                  UnknownBoardType,                      ///< Issue class name
                  " Unknown board type: " << board_type, ///< Message
                  ((std::string)board_type)               ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                       ///< Namespace
                  UnknownCarrierType,                        ///< Issue class name
                  " Unknown carrier type: " << carrier_type, ///< Message
                  ((std::string)carrier_type)                ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                     ///< Namespace
                  UnknownDesignType,                       ///< Issue class name
                  " Unknown design type: " << design_type, ///< Message
                  ((std::string)design_type)               ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                                                           ///< Namespace
                  UnknownBoardUID,                                                                   ///< Issue class name
                  " Unknown UID-board revision mapping: " << board_uid, ///< Message
                  ((std::string)board_uid)                                                                ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                                                           ///< Namespace
                  MissingBoardTypeMapEntry,                                                                   ///< Issue class name
                  " Board type not in board type map: " << board_type, ///< Message
                  ((std::string)board_type)                                                                ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                                                           ///< Namespace
                  MissingBoardRevisionMapEntry,                                                                   ///< Issue class name
                  " Board revision not in board revision map: " << board_rev, ///< Message
                  ((std::string)board_rev)                                                                ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                                                           ///< Namespace
                  MissingCarrierTypeMapEntry,                                                                   ///< Issue class name
                  " Carrier type not in carrier type map: " << carrier_type, ///< Message
                  ((std::string)carrier_type)                                                                ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                                                           ///< Namespace
                  MissingDesignTypeMapEntry,                                                                   ///< Issue class name
                  " Design type not in design type map: " << design_type, ///< Message
                  ((std::string)design_type)                                                                ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                                                           ///< Namespace
                  ClockConfigNotFound,                                                                   ///< Issue class name
                  " Clock config file not found for key: " << clock_key, ///< Message
                  ((std::string)clock_key)                                                                ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                           ///< Namespace
                  InvalidDACId,                  ///< Issue class name
                  " Invalid DAC ID: " << dac_id, ///< Message
                  ((std::string)dac_id)          ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                                                           ///< Namespace
                  EchoTimeout,                                                                   ///< Issue class name
                  " Timeout whilst waiting for echo. Timeout (ms): " << std::to_string(timeout), ///< Message
                  ((uint)timeout)                                                                ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                                  ///< Namespace
                  FormatCountersTableNodesTitlesMismatch,               ///< Issue class name
                  " Mismatch between number counters nodes and titles", ///< Message
                  ERS_EMPTY                                             ///< Message parameters
)

// SFP related issues
ERS_DECLARE_ISSUE(pdt,                           ///< Namespace
                  InvalidSFPId,                  ///< Issue class name
                  " Invalid SFP ID: " << sfp_id, ///< Message
                  ((std::string)sfp_id)          ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                ///< Namespace
                  SFPUnreachable,    ///< Issue class name
                  " Failed to reach SFP on I2C bus: " << bus_id, ///< Message
                  ((std::string)bus_id)               ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                                      ///< Namespace
                  SFPDDMUnsupported,                                        ///< Issue class name
                  " SFP on I2C bus: " << bus_id << " does not support DDM", ///< Message
                  ((std::string)bus_id)                                     ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                ///< Namespace
                  SFPDDMI2CAddressSwapUnsupported,    ///< Issue class name
                  " SFP DDM I2C address swap not supported. SFP on I2C bus: " << bus_id, ///< Message
                  ((std::string)bus_id)               ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                ///< Namespace
                  SoftTxLaserControlUnsupported,      ///< Issue class name
                  " SFP on I2C bus: " << bus_id << " does not support soft tx laser control", ///< Message
                  ((std::string)bus_id)               ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                    ///< Namespace
                  UnsupportedFunction,    ///< Issue class name
                  message,                ///< Message
                  ((std::string)message)  ///< Message parameters
)
ERS_DECLARE_ISSUE(pdt,                                                                                                             ///< Namespace
                  BadRequestedFakeTriggerRate,                                                                                     ///< Issue class name
                  " Requested trigger rate: " << std::to_string(trig_rate) << ", ps: " << std::to_string(ps) << ", out of range.", ///< Message
                  ((double)trig_rate)((uint)ps)                                                                                    ///< Message parameters
)

ERS_DECLARE_ISSUE(pdt,                                                                  ///< Namespace
                  UpstreamEndpointFailedToLock,                                         ///< Issue class name
                  " Failed to bring up the RTT endpoint. Current state: " << ept_state, ///< Message
                  ((std::string)ept_state)                                               ///< Message parameters
)

} // namespace dunedaq

#endif // TIMING_BOARD_SOFTWARE_SRC_TIMING_ISSUES_HPP_