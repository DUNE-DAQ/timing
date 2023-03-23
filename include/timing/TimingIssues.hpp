/**
 * @file TimingNode.hpp
 *
 * Timing ers issues.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_TIMINGISSUES_HPP_
#define TIMING_INCLUDE_TIMING_TIMINGISSUES_HPP_

#include "ers/Issue.hpp"

#include <string>

namespace dunedaq {

// Generic exceptions
ERS_DECLARE_ISSUE(timing,                          ///< Namespace
                  FileNotFound,                    ///< Issue class name
                  file_path << " does not exist!", ///< Message
                  ((std::string)file_path)         ///< Message parameters
)
ERS_DECLARE_ISSUE(timing,                    ///< Namespace
                  CorruptedFile,             ///< Issue class name
                  file_path << " corrupted", ///< Message
                  ((std::string)file_path)   ///< Message parameters
)
ERS_DECLARE_ISSUE_BASE(timing,                         ///< Namespace
                       FileIsDirectory,                ///< Issue class name
                       CorruptedFile,                  ///< Base class of the issue
                       file_path << " is a directory", ///< Log Message from the issue
                       ((std::string)file_path),       ///< Base class attributes
                       ERS_EMPTY                       ///< Attribute of this class
)

ERS_DECLARE_ISSUE(timing,                                            ///< Namespace
                  EnvironmentVariableNotSet,                         ///< Issue class name
                  "Environment variable: " << env_var << " not set", ///< Message
                  ((std::string)env_var)                             ///< Message parameters
)

// I2C related issues
ERS_DECLARE_ISSUE(timing,                             ///< Namespace
                  I2CException,                       ///< Issue class name
                  " I2CException on bus: " << bus_id, ///< Message
                  ((std::string)bus_id)               ///< Message parameters
)

ERS_DECLARE_ISSUE_BASE(timing,                                                          ///< Namespace
                       I2CDeviceNotFound,                                               ///< Issue class name
                       I2CException,                                                    ///< Base class of the issue
                       " I2C device: " << device_id << " not found on bus: " << bus_id, ///< Log Message from the issue
                       ((std::string)bus_id),                                           ///< Base class attributes
                       ((std::string)device_id)                                         ///< Attribute of this class
)

ERS_DECLARE_ISSUE_BASE(
  timing,                                                                         ///< Namespace
  I2CBusArbitrationLost,                                                          ///< Issue class name
  I2CException,                                                                   ///< Base class of the issue
  " I2C bus: " << bus_id << " arbitration lost. Is another application running?", ///< Log Message from the issue
  ((std::string)bus_id),                                                          ///< Base class attributes
  ERS_EMPTY                                                                       ///< Attribute of this class
)

ERS_DECLARE_ISSUE_BASE(
  timing,                ///< Namespace
  I2CTransactionTimeout, ///< Issue class name
  I2CException,          ///< Base class of the issue
  " I2C bus: "
    << bus_id
    << " error. Transaction timeout - the 'Transfer in Progress' bit remained high for too long", ///< Log Message from
                                                                                                  ///< the issue
  ((std::string)bus_id), ///< Base class attributes
  ERS_EMPTY              ///< Attribute of this class
)

ERS_DECLARE_ISSUE_BASE(timing,                                                      ///< Namespace
                       I2CNoAcknowledgeReceived,                                    ///< Issue class name
                       I2CException,                                                ///< Base class of the issue
                       " I2C bus: " << bus_id << " error. No acknowledge received", ///< Log Message from the issue
                       ((std::string)bus_id),                                       ///< Base class attributes
                       ERS_EMPTY                                                    ///< Attribute of this class
)

ERS_DECLARE_ISSUE_BASE(timing,                          ///< Namespace
                       I2CTransferFinishedBusStillBusy, ///< Issue class name
                       I2CException,                    ///< Base class of the issue
                       " I2C bus: " << bus_id
                                    << " error. Transfer finished but bus still busy", ///< Log Message from the issue
                       ((std::string)bus_id),                                          ///< Base class attributes
                       ERS_EMPTY                                                       ///< Attribute of this class
)

ERS_DECLARE_ISSUE(timing,                                ///< Namespace
                  UnknownBoardType,                      ///< Issue class name
                  " Unknown board type: " << board_type, ///< Message
                  ((std::string)board_type)              ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                    ///< Namespace
                  UnknownCarrierType,                        ///< Issue class name
                  " Unknown carrier type: " << carrier_type, ///< Message
                  ((std::string)carrier_type)                ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                  ///< Namespace
                  UnknownDesignType,                       ///< Issue class name
                  " Unknown design type: " << design_type, ///< Message
                  ((std::string)design_type)               ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                               ///< Namespace
                  UnknownBoardUID,                                      ///< Issue class name
                  " Unknown UID-board revision mapping: " << board_uid, ///< Message
                  ((std::string)board_uid)                              ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                              ///< Namespace
                  UnknownFirmwareClockFrequency,                                       ///< Issue class name
                  frequency << " Hz is not a known timing firmwaare clock frequency!", ///< Message
                  ((std::uint32_t)frequency)                                           // NOLINT(build/unsigned) /< Message parameters 
)

ERS_DECLARE_ISSUE(timing,                                              ///< Namespace
                  MissingBoardTypeMapEntry,                            ///< Issue class name
                  " Board type not in board type map: " << board_type, ///< Message
                  ((std::string)board_type)                            ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                     ///< Namespace
                  MissingBoardRevisionMapEntry,                               ///< Issue class name
                  " Board revision not in board revision map: " << board_rev, ///< Message
                  ((std::string)board_rev)                                    ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                    ///< Namespace
                  MissingCarrierTypeMapEntry,                                ///< Issue class name
                  " Carrier type not in carrier type map: " << carrier_type, ///< Message
                  ((std::string)carrier_type)                                ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                 ///< Namespace
                  MissingDesignTypeMapEntry,                              ///< Issue class name
                  " Design type not in design type map: " << design_type, ///< Message
                  ((std::string)design_type)                              ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                ///< Namespace
                  ClockConfigNotFound,                                   ///< Issue class name
                  " Clock config file not found for key: " << clock_key, ///< Message
                  ((std::string)clock_key)                               ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                        ///< Namespace
                  InvalidDACId,                  ///< Issue class name
                  " Invalid DAC ID: " << dac_id, ///< Message
                  ((std::string)dac_id)          ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                    ///< Namespace
                  EchoReplyTimeout,                          ///< Issue class name
                  " Timeout whilst waiting for echo reply.", ///< Message
                  ERS_EMPTY                                  ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                                   ///< Namespace
                  EchoFlagTimeout,                                                                          ///< Issue class name
                  " Timeout whilst waiting for echo reply flag. Timeout (ms): " << std::to_string(timeout), ///< Message
                  ((uint)timeout)                                                                           ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                               ///< Namespace
                  VLCommandReplyTimeout,                                                ///< Issue class name
                  " Timeout whilst waiting for variable length (async) command reply.", ///< Message
                  ERS_EMPTY                                                             ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                                                                          ///< Namespace
                  VLCommandReplyBufferFlagTimeout,                                                                                                 ///< Issue class name
                  " Timeout whilst waiting for variable length (async) reply buffer response flag set. Timeout (ms): " << std::to_string(timeout), ///< Message
                   ((uint)timeout)                                                                                                                 ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                                                                                      ///< Namespace
                  InvalidVLCommandReplyPacket,                                                                                                                 ///< Issue class name
                  " Variable length (async) command reply packet invalid. byte 0,1,2: " << std::hex << "0x" << byte_0 << ", 0x" << byte_1 << ", 0x" << byte_2, ///< Message
                  ((uint32_t)byte_0)((uint32_t)byte_1)((uint32_t)byte_2)                                                                                       ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                               ///< Namespace
                  FormatCountersTableNodesTitlesMismatch,               ///< Issue class name
                  " Mismatch between number counters nodes and titles", ///< Message
                  ERS_EMPTY                                             ///< Message parameters
)

// SFP related issues
ERS_DECLARE_ISSUE(timing,                        ///< Namespace
                  InvalidSFPId,                  ///< Issue class name
                  " Invalid SFP ID: " << sfp_id, ///< Message
                  ((std::string)sfp_id)          ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                        ///< Namespace
                  SFPUnreachable,                                ///< Issue class name
                  " Failed to reach SFP on I2C bus: " << bus_id, ///< Message
                  ((std::string)bus_id)                          ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                   ///< Namespace
                  SFPDDMUnsupported,                                        ///< Issue class name
                  " SFP on I2C bus: " << bus_id << " does not support DDM", ///< Message
                  ((std::string)bus_id)                                     ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                ///< Namespace
                  SFPDDMI2CAddressSwapUnsupported,                                       ///< Issue class name
                  " SFP DDM I2C address swap not supported. SFP on I2C bus: " << bus_id, ///< Message
                  ((std::string)bus_id)                                                  ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                     ///< Namespace
                  SoftTxLaserControlUnsupported,                                              ///< Issue class name
                  " SFP on I2C bus: " << bus_id << " does not support soft tx laser control", ///< Message
                  ((std::string)bus_id)                                                       ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                ///< Namespace
                  UnsupportedFunction,   ///< Issue class name
                  message,               ///< Message
                  ((std::string)message) ///< Message parameters
)
ERS_DECLARE_ISSUE(timing,                      ///< Namespace
                  BadRequestedFakeTriggerRate, ///< Issue class name
                  " Requested trigger rate: " << std::to_string(trig_rate) << ", ps: " << std::to_string(ps)
                                              << ", out of range.", ///< Message
                  ((double)trig_rate)((uint)ps)                     ///< Message parameters // NOLINT
)

ERS_DECLARE_ISSUE(timing,                                                                 ///< Namespace
                  EndpointNotReady,                                                       ///< Issue class name
                  ept_description << " endpoint not ready. Current state: 0x" << std::hex << ept_state, ///< Message
                  ((std::string)ept_description)((uint32_t)ept_state)                  ///< Message parameters
)

ERS_DECLARE_ISSUE(timing, HSIBufferIssue, "HSI buffer in state: " << buffer_state, ((std::string)buffer_state))

ERS_DECLARE_ISSUE(timing,                                                                       ///< Namespace
                  EnclustraSwitchFailure,                                                       ///< Issue class name
                  " Failed to program Enclustra I2C IO expander. FMC I2C access may not work.", ///< Message
                  ERS_EMPTY)                                                                    ///< Message parameters

ERS_DECLARE_ISSUE(timing,                                                                                                                            ///< Namespace
                  IncompatibleMajorMasterFirmwareVersion,                                                                                            ///< Issue class name
                  " Incompatible major master firmware version; found: " << found_firmware_version << ", required: " << required_firmware_version, ///< Message
                  ((int)found_firmware_version)((int)required_firmware_version)                                                                      ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                                                                            ///< Namespace
                  IncompatibleMinorMasterFirmwareVersion,                                                                                            ///< Issue class name
                  " Incompatible minor master firmware version; found: " << found_firmware_version << ", required: " << required_firmware_version, ///< Message
                  ((int)found_firmware_version)((int)required_firmware_version)                                                                      ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                                                                            ///< Namespace
                  IncompatiblePatchMasterFirmwareVersion,                                                                                            ///< Issue class name
                  " Incompatible patch master firmware version; found: " << found_firmware_version << ", required: " << required_firmware_version, ///< Message
                  ((int)found_firmware_version)((int)required_firmware_version)                                                                      ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                                                                            ///< Namespace
                  IncompatibleMajorEndpointFirmwareVersion,                                                                                          ///< Issue class name
                  " Incompatible major endpoint firmware version; found: " << found_firmware_version << ", required: " << required_firmware_version, ///< Message
                  ((int)found_firmware_version)((int)required_firmware_version)                                                                      ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                                                                            ///< Namespace
                  IncompatibleMinorEndpointFirmwareVersion,                                                                                          ///< Issue class name
                  " Incompatible minor endpoint firmware version; found: " << found_firmware_version << ", required: " << required_firmware_version, ///< Message
                  ((int)found_firmware_version)((int)required_firmware_version)                                                                      ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                                                                            ///< Namespace
                  IncompatiblePatchEndpointFirmwareVersion,                                                                                          ///< Issue class name
                  " Incompatible patch endpoint firmware version; found: " << found_firmware_version << ", required: " << required_firmware_version, ///< Message
                  ((int)found_firmware_version)((int)required_firmware_version)                                                                      ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                       ///< Namespace
                  FailedToUpdateHSIRandomRate,                                                  ///< Issue class name
                  "  Random bit 0 trigger rate for HSI not updated!",                           ///< Message
                  ERS_EMPTY)                                                                    //< Message parameters

ERS_DECLARE_ISSUE(timing,                          ///< Namespace
                  InvalidAMCSlot,                  ///< Issue class name
                  " Invalid AMC slot: " << amc_slot, ///< Message
                  ((std::string)amc_slot)          ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                                    ///< Namespace
                  InvalidFixedLatencyCommand,                                                                ///< Issue class name
                  " Supplied fixed-latency broadcast command, 0x" << std::hex <<  command << " is invalid" , ///< Message
                   ((uint)command)                                                                           ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                                    ///< Namespace
                  InvalidFixedLatencyCommandChannel,                                                                ///< Issue class name
                  " Supplied fixed-latency broadcast command channel, 0x" << std::hex <<  channel << " is invalid" , ///< Message
                   ((uint)channel)                                                                           ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                              ///< Namespace
                  MonitoredEndpointDead,                                                               ///< Issue class name
                  "Monitored endpoint at address 0x" << std::hex << ept_address << " did not respond", ///< Message
                  ((uint16_t)ept_address)                                                              ///< Message parameters
)

ERS_DECLARE_ISSUE(timing,                                                                                                         ///< Namespace
                  MonitoredEndpointUnexpectedState,                                                                               ///< Issue class name
                  "Monitored endpoint at address 0x" << std::hex << ept_address << " is in an unexpected state: 0x" << ept_state, ///< Message
                  ((uint16_t)ept_address)((uint8_t)ept_state)                                                                     ///< Message parameters
)
} // namespace dunedaq

#endif // TIMING_INCLUDE_TIMING_TIMINGISSUES_HPP_