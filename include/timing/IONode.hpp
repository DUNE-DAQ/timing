/**
 * @file IONode.hpp
 *
 * IONode is a base class providing an interface
 * to for IO firmware blocks.
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TIMING_INCLUDE_TIMING_IONODE_HPP_
#define TIMING_INCLUDE_TIMING_IONODE_HPP_

// PDT Headers
#include "timing/TimingNode.hpp"

#include "TimingIssues.hpp"

#include "timing/DACNode.hpp"
#include "timing/FrequencyCounterNode.hpp"
#include "timing/I2CExpanderNode.hpp"
#include "timing/I2CMasterNode.hpp"
#include "timing/I2CSFPNode.hpp"
#include "timing/I2CSlave.hpp"
#include "timing/SI534xNode.hpp"

// uHal Headers
#include "uhal/DerivedNode.hpp"

// C++ Headers
#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace dunedaq {
namespace timing {

/**
 * @brief      Base class for timing IO nodes.
 */
class IONode : public TimingNode
{

public:
  IONode(const uhal::Node& node,
         std::string uid_i2c_bus,
         std::string pll_i2c_bus,
         std::string pll_i2c_device,
         std::vector<std::string> clock_names,
         std::vector<std::string> sfp_i2c_buses);
  virtual ~IONode();

  /**
   * @brief      Get the UID address parameter name.
   *
   * @return     { description_of_the_return_value }
   */
  virtual std::string get_uid_address_parameter_name() const = 0;

  /**
   * @brief      Read the word identifying the timing board.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_board_type() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the word identifying the FPFA carrier board.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_carrier_type() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the word identifying the firmware design in the FPGA.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_design_type() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the word identifying the frequency [units of Hz] of the firmware in the FPGA.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint32_t read_firmware_frequency() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the word containing the timing board UID.
   *
   * @return     { description_of_the_return_value }
   */
  virtual uint64_t read_board_uid() const; // NOLINT(build/unsigned)

  /**
   * @brief      Read the word identifying the timing board.
   *
   * @return     { description_of_the_return_value }
   */
  virtual BoardRevision get_board_revision() const;

  /**
   * @brief      Print hardware information
   */
  virtual std::string get_hardware_info(bool print_out = false) const;

  /**
   * @brief      Get the full config path.
   *
   * @return     { description_of_the_return_value }
   */
  virtual std::string get_full_clock_config_file_path(const ClockSource& clock_source) const;

  /**
   * @brief      Get the an I2C chip.
   *
   * @return     { description_of_the_return_value }
   */
  template<class T>
  std::unique_ptr<const T> get_i2c_device(const std::string& i2c_bus_name, const std::string& i2c_device_name) const;

  /**
   * @brief      Get the PLL chip.
   *
   * @return     { description_of_the_return_value }
   */
  virtual std::unique_ptr<const SI534xSlave> get_pll() const;

  /**
   * @brief      Configure clock chip.
   */
  virtual void configure_pll(const std::string& clock_config_file = "") const;

  /**
   * @brief      Read frequencies of on-board clocks.
   */
  virtual std::vector<double> read_clock_frequencies() const;

  /**
   * @brief      Print frequencies of on-board clocks.
   */
  virtual std::string get_clock_frequencies_table(bool print_out = false) const;

  /**
   * @brief      Print status of on-board PLL.
   */
  virtual std::string get_pll_status(bool print_out = false) const;

  /**
   * @brief      Print status of on-board SFP.
   */
  virtual std::string get_sfp_status(uint32_t sfp_id, bool print_out = false) const; // NOLINT(build/unsigned)

  /**
   * @brief      control tx laser of on-board SFP softly (I2C command)
   */
  virtual void switch_sfp_soft_tx_control_bit(uint32_t sfp_id, bool turn_on) const; // NOLINT(build/unsigned)

  /**
   * @brief      Reset timing node.
   */
  virtual void soft_reset() const;

  /**
   * @brief      Reset timing node.
   */
  virtual void reset(const std::string& clock_config_file) const = 0;

  /**
   * @brief     Reset timing node with clock file lookup
   */
  virtual void reset(const ClockSource& clock_source) const;

  static const std::map<BoardType, std::string>& get_board_type_map() { return board_type_map; }

  static const std::map<CarrierType, std::string>& get_carrier_type_map() { return carrier_type_map; }

  static const std::map<BoardRevision, std::string>& get_board_revision_map() { return board_revision_map; }
  static const std::map<uint64_t, BoardRevision>& get_board_uid_revision_map() { return board_uid_revision_map; }
  static const std::map<DesignType, std::string>& get_design_type_map() { return design_type_map; }
  static const std::map<std::string, std::string>& get_clock_config_map() { return clock_config_map; }
  static const std::vector<BoardType>& get_library_supported_boards() { return library_supported_boards; }
  static const std::vector<DesignType>& get_library_supported_designs() { return library_supported_designs; }

protected:
  const std::string m_uid_i2c_bus;
  const std::string m_pll_i2c_bus;
  const std::string m_pll_i2c_device;
  const std::vector<std::string> m_clock_names;
  const std::vector<std::string> m_sfp_i2c_buses;

  /**
   * @brief      Write soft reset register.
   */
  virtual void write_soft_reset_register() const;

  static inline const std::map<BoardType, std::string> board_type_map = { { kBoardFMC, "fmc" },
                                                            { kBoardSim, "sim" },
                                                            { kBoardPC059, "pc059" },
                                                            { kBoardMicrozed, "microzed" },
                                                            { kBoardTLU, "tlu" },
                                                            { kBoardFIB, "fib" },
                                                            { kBoardMIB, "mib" },
                                                            { kBoardPC069, "pc069" },
                                                            { kBoardGIB, "gib" },
                                                            { kBoardUnknown, "unknown" }
                                                            };

  static inline const std::map<CarrierType, std::string> carrier_type_map = { { kCarrierEnclustraA35, "enclustra-a35" },
                                                                { kCarrierKC705, "kc705" },
                                                                { kCarrierMicrozed, "microzed" },
                                                                { kCarrierATFC, "atfc" },
                                                                { kCarrierAFC, "afc" },
                                                                { kCarrierNexusVideo, "nexus-video" },
                                                                { kCarrierTrenzTE0712, "trenz-te0712" },
                                                                { kCarrierUnknown, "unknown" }
                                                                };
  static inline const std::map<BoardRevision, std::string> board_revision_map = {
                                                                { kFMCRev1, "kFMCRev1" },     { kFMCRev2, "kFMCRev2" }, { kFMCRev3, "kFMCRev3" },
                                                                { kPC059Rev1, "kPC059Rev1" }, { kTLURev1, "kTLURev1" }, { kSIMRev1, "kSIMRev1" },
                                                                { kFIBRev1, "kFIBRev1" }, { kFMCRev4, "kFMCRev4" }, { kMIBRev1, "kMIBRev1" },
                                                                { kPC069a, "pc069a" }, { kPC069b, "pc069b" }, { kMIBRev2, "kMIBRev2" }, { kGIBRev1, "kGIBRev1" },
                                                                { kBoardRevisionUnknown, "unknown" }
  };

  static inline const std::map<DesignType, std::string> design_type_map = {
                                                                { kDesignMaster, "master" },
                                                                { kDesignOuroboros, "ouroboros" },
                                                                { kDesignOuroborosSim, "ouroboros-sim" },
                                                                { kDesignTest, "test-design" },
                                                                { kDesignEndpoint, "endpoint" },
                                                                { kDesignFanout, "fanout" },
                                                                { kDesignOverlord, "overlord" },
                                                                { kDesignEndpoBICRT, "endpoint-bi-crt" },
                                                                { kDesignChronos, "chronos" },
                                                                { kDesignBoreas, "boreas" },
                                                                { kDesignKerberos, "kerberos" },
                                                                { kDesignGaia, "gaia" },
                                                                { kDesignUnknown, "unknown" },
  };

  // NOLINTNEXTLINE(build/unsigned)
  static inline const std::map<uint64_t, BoardRevision> board_uid_revision_map = {
                                                                    { 0xd880395e720b, kFMCRev1 },
                                                                    { 0xd880395e501a, kFMCRev1 },
                                                                    { 0xd880395e50b8, kFMCRev1 },
                                                                    { 0xd880395e501b, kFMCRev1 },
                                                                    { 0xd880395e7201, kFMCRev1 },
                                                                    { 0xd880395e4fcc, kFMCRev1 },
                                                                    { 0xd880395e5069, kFMCRev1 },
                                                                    { 0xd880395e7206, kFMCRev1 },
                                                                    { 0xd880395e1c86, kFMCRev2 },
                                                                    { 0xd880395e2630, kFMCRev2 },
                                                                    { 0xd880395e262b, kFMCRev2 },
                                                                    { 0xd880395e2b38, kFMCRev2 },
                                                                    { 0xd880395e1a6a, kFMCRev2 },
                                                                    { 0xd880395e36ae, kFMCRev2 },
                                                                    { 0xd880395e2b2e, kFMCRev2 },
                                                                    { 0xd880395e2b33, kFMCRev2 },
                                                                    { 0xd880395e1c81, kFMCRev2 },
                                                                    { 0x049162b67ce6, kFMCRev3 },
                                                                    { 0x049162b62947, kFMCRev3 },
                                                                    { 0x049162b67cdf, kFMCRev3 },
                                                                    { 0x49162b62050, kFMCRev3 },
                                                                    { 0x49162b62951, kFMCRev3 },
                                                                    { 0x49162b675e3, kFMCRev3 },
                                                                    { 0xd88039d980cf, kPC059Rev1 },
                                                                    { 0xd88039d98adf, kPC059Rev1 },
                                                                    { 0xd88039d92491, kPC059Rev1 },
                                                                    { 0xd88039d9248e, kPC059Rev1 },
                                                                    { 0xd88039d98ae9, kPC059Rev1 },
                                                                    { 0xd88039d92498, kPC059Rev1 },
                                                                    { 0x5410ecbba408, kTLURev1 },
                                                                    { 0x5410ecbb9426, kTLURev1 },
                                                                    { 0x801f12f5ce48, kFIBRev1 },
                                                                    { 0x801f12f5e9ae, kFIBRev1 },
                                                                    { 0x49162b65025, kFMCRev3 },
                                                                    { 0x49162b62948, kFMCRev3 },
                                                                    { 0x49162b675ea, kFMCRev3 },
                                                                    { 0x49162b645cd, kFMCRev3 },
                                                                    { 0xd880395dab52, kFMCRev4 },
                                                                    { 0xd880395d99b3, kFMCRev4 },
                                                                    { 0xd880395df010, kFMCRev4 },
                                                                    { 0xd880395df00f, kFMCRev4 },
                                                                    { 0xd880395de4a4, kFMCRev4 },
                                                                    { 0xd880395d99a9, kFMCRev4 },
                                                                    { 0xd880395d99b0, kFMCRev4 },
                                                                    { 0xd880395de452, kFMCRev4 },
                                                                    { 0xd880395da48e, kFMCRev4 },
                                                                    { 0xd880395dbcee, kFMCRev4 },
                                                                    { 0x803428749c04, kPC069a  },
                                                                    { 0x5410ecbb6845, kTLURev1 },
                                                                    { 0x801f12ee6739, kMIBRev1 },
                                                                    { 0x801f12f5e183, kFIBRev1 },
                                                                    { 0x49162b1d910,  kMIBRev2 },
                                                                    { 0x801f12ee56f3, kGIBRev1 },
                                                                    { 0x80342874c33d, kPC069a },
                                                                    { 0x803428748902, kPC069a },
                                                                    { 0x8034287498ea, kPC069a },
                                                                    { 0xfc0fe700bece, kPC069b },
                                                                    { 0xfc0fe700c286, kPC069b },
                                                                    { 0xfc0fe700d293, kPC069b },
                                                                    { 0xfc0fe700bec4, kPC069b },
                                                                    { 0xfc0fe700c281, kPC069b },
                                                                    { 0xfc0fe700bec9, kPC069b },
                                                                    { 0xfc0fe700d28e, kPC069b },
                                                                    { 0xfc0fe700d289, kPC069b },
                                                                    { 0xfc0fe700d298, kPC069b },
                                                                    { 0xfc0fe700c28b, kPC069b },
  };

  static inline const std::map<ClockSource, std::string> clock_source_map = {
                                                            { kFreeRun, "free_run" },
                                                            { kInput0, "input_0" },
                                                            { kInput1, "input_1" },
                                                            { kInput2, "input_2" },
                                                            { kInput3, "input_3" },
                                                            };

  static inline const std::map<std::string, std::string> clock_config_map = {

        // 62.5 MHz mappings (no cdr)
        { "fmc_5344_ouroboros_free_run", "SI5344/PDTS0003.txt" },
        { "fmc_5394_ouroboros_free_run", "SI5344/PDTS0003.txt" },

        { "fmc_5344_master_free_run", "SI5344/PDTS0003.txt" },
        { "fmc_5394_master_free_run", "SI5344/PDTS0003.txt" },

        { "fmc_5344_overlord_free_run", "SI5344/PDTS0003.txt" },
        { "fmc_5394_overlord_free_run", "SI5344/PDTS0003.txt" },

        { "fmc_5344_boreas_free_run", "SI5344/PDTS0003.txt" },
        { "fmc_5394_boreas_free_run", "SI5344/PDTS0003.txt" },

        { "fmc_5344_endpoint_input_1", "devel/ENDPOINT-Si5344-50MHzRef.txt" },
        { "fmc_5394_endpoint_input_1", "devel/ENDPOINT-Si5344-50MHzRef.txt" },

        { "fmc_5344_chronos_input_1", "devel/ENDPOINT-Si5344-50MHzRef.txt" },
        { "fmc_5394_chronos_input_1", "devel/ENDPOINT-Si5344-50MHzRef.txt" },

        { "tlu_5345_boreas_free_run", "nocdr/DUNE_TLU-4_45_M-Registers.txt" },

        { "pc059_5345_master_free_run", "devel/PDTS_PC059_FANOUT.txt" },

        { "pc069_5395_master_free_run", "nocdr/Si5395-RevA-069a_mst-Registers.txt" },
        { "pc069_5395_boreas_free_run", "nocdr/Si5395-RevA-069a_mst-Registers.txt" },

        { "pc069_5395_endpoint_input_1", "nocdr/Si5395-RevA-069a_ep-Registers.txt" },
        { "pc069_5395_chronos_input_1", "nocdr/Si5395-RevA-069a_ep-Registers.txt" },

        { "pc069_5345_master_free_run", "nocdr/Si5345-RevD-pc69bmst-Registers.txt" },
        { "pc069_5345_boreas_free_run", "nocdr/Si5345-RevD-pc69bmst-Registers.txt" },

        { "pc069_5345_chronos_input_1", "nocdr/Si5345-RevD-pc69bept-Registers.txt" },
        { "pc069_5345_charon_input_1", "nocdr/Si5345-RevD-pc69bept-Registers.txt" },

        { "gib_5395_gaia_free_run", "nocdr/GIB_Debug_01.txt" },

        { "gib_5395_gaia_input_0", "nocdr/Si5395-RevA-GIB10MHZ-Registers.txt" },

        { "mib_5395_kerberos_free_run",
          "nocdr/Si5395-RevA-MIB_FREE-Registers.txt" },
        { "mib_5395_kerberos_input_0",
          "nocdr/Si5395-RevA-MIB_INSW-Registers.txt" },
        { "mib_5395_kerberos_input_1",
          "nocdr/Si5395-RevA-MIB_INSW-Registers.txt" },
        { "mib_5395_kerberos_input_2",
          "nocdr/Si5395-RevA-MIB_INSW-Registers.txt" },

        { "fib_5395_fanout_input_1",
          "nocdr/Si5395-RevA-FIB_FAN_AFCv4-FIBBP250-Registers.txt" }, // fanout design, data and clock from backplane
  };

  static inline const std::vector<BoardType> library_supported_boards = {
    kBoardFMC, kBoardPC059, kBoardTLU, kBoardSim, kBoardFIB, kBoardMIB, kBoardPC069, kBoardGIB
  };

  static inline const std::vector<DesignType> library_supported_designs = {
    kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignOverlord, kDesignEndpoBICRT, kDesignChronos, kDesignBoreas, kDesignKerberos, kDesignGaia
  };

  static inline std::string clock_source_to_string(const ClockSource& source)
  {
     if (auto source_it = clock_source_map.find(source); source_it != clock_source_map.end())
     {
      return source_it->second;
     }
     else
     {
      throw MissingClockSourceMapEntry(ERS_HERE, format_reg_value(source));
     }
  }

};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/IONode.hxx"

#endif // TIMING_INCLUDE_TIMING_IONODE_HPP_