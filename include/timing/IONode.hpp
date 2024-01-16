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
  virtual std::string get_full_clock_config_file_path(const std::string& clock_config_file, int32_t mode = -1) const;

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
  virtual void reset(const std::string& clock_config_file = "") const = 0;

  /**
   * @brief     Reset fanout board
   */
  virtual void reset(int32_t fanout_mode, // NOLINT(build/unsigned)
                     const std::string& clock_config_file = "") const = 0;

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

  static inline const std::map<std::string, std::string> clock_config_map = {

        // 62.5 MHz mappings (no cdr)
        { "fmc_5344_ouroboros", "SI5344/PDTS0003.txt" },
        { "fmc_5394_ouroboros", "SI5344/PDTS0003.txt" },

        { "fmc_5344_master", "SI5344/PDTS0003.txt" },
        { "fmc_5394_master", "SI5344/PDTS0003.txt" },

        { "fmc_5344_overlord", "SI5344/PDTS0003.txt" },
        { "fmc_5394_overlord", "SI5344/PDTS0003.txt" },

        { "fmc_5344_boreas", "SI5344/PDTS0003.txt" },
        { "fmc_5394_boreas", "SI5344/PDTS0003.txt" },

        { "fmc_5344_endpoint", "devel/ENDPOINT-Si5344-50MHzRef.txt" },
        { "fmc_5394_endpoint", "devel/ENDPOINT-Si5344-50MHzRef.txt" },

        { "fmc_5344_chronos", "devel/ENDPOINT-Si5344-50MHzRef.txt" },
        { "fmc_5394_chronos", "devel/ENDPOINT-Si5344-50MHzRef.txt" },

        { "tlu_5345_boreas", "nocdr/DUNE_TLU-4_45_M-Registers.txt" },

        { "pc059_5345_master", "devel/PDTS_PC059_FANOUT.txt" },

        { "pc069_5395_master", "nocdr/Si5395-RevA-069a_mst-Registers.txt" },
        { "pc069_5395_boreas", "nocdr/Si5395-RevA-069a_mst-Registers.txt" },

        { "pc069_5395_endpoint", "nocdr/Si5395-RevA-069a_ep-Registers.txt" },
        { "pc069_5395_chronos", "nocdr/Si5395-RevA-069a_ep-Registers.txt" },

        { "gib_5395_gaia", "nocdr/GIB_Debug_01.txt" },

        { "fib_5395_fanout_mode0",
          "nocdr/Si5395-RevA-FIB_FAN_AFCv4-FIBBP250-Registers.txt" }, // fanout mode, data and clock from backplane

        { "pc069_5345_master", "nocdr/Si5345-RevD-pc69bmst-Registers.txt" },
        { "pc069_5345_boreas", "nocdr/Si5345-RevD-pc69bmst-Registers.txt" },

        { "pc069_5345_chronos", "nocdr/Si5345-RevD-pc69bept-Registers.txt" },
        { "pc069_5345_charon", "nocdr/Si5345-RevD-pc69bept-Registers.txt" },

        // 62.5 MHz mappings (with cdr)
        { "fmc_5344_endpoint_cdr", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },
        { "fmc_5394_endpoint_cdr", "devel/Si5394-053endptr_62-5MHz_4kHz-Registers.txt" },

        { "fmc_5344_endpoint-bi-crt_cdr", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },
        { "fmc_5394_endpoint-bi-crt_cdr", "devel/Si5394-053endptr_62-5MHz_4kHz-Registers.txt" },

        { "fmc_5344_chronos_cdr", "devel/endpoint_si5344_312_mhz-e_44_312-Registers.txt" },
        { "fmc_5394_chronos_cdr", "devel/Si5394-053endptr_62-5MHz_4kHz-Registers.txt" },

        { "fmc_5344_ouroboros_cdr", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
        { "fmc_5394_ouroboros_cdr", "devel/Si5394-RevA-94mst625-Registers.txt" },

        { "fmc_5344_master_cdr", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
        { "fmc_5394_master_cdr", "devel/Si5394-RevA-94mst625-Registers.txt" },

        { "fmc_5344_overlord_cdr", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
        { "fmc_5394_overlord_cdr", "devel/Si5394-RevA-94mst625-Registers.txt" },

        { "fmc_5344_boreas_cdr", "devel/Si5344-053master_312.5_mhz-Registers.txt" },
        { "fmc_5394_boreas_cdr", "devel/Si5394-RevA-94mst625-Registers.txt" },

        { "tlu_5345_overlord_cdr", "devel/DUNE_TLU-DUNTLU09-Registers_62.5_mhz.txt" },
        { "tlu_5345_boreas_cdr", "devel/DUNE_TLU-DUNTLU09-Registers_62.5_mhz.txt" },

        { "pc059_5345_ouroboros_cdr", "devel/pc059_standalone_312_mhz-059_1_62-Registers.txt" },
        { "pc059_5345_fanout_mode0_cdr",
          "devel/pc059_sfp_in_312_mhz-059_0s62-Registers.txt" }, // fanout mode, assuming sfp is the upstream input
        { "pc059_5345_fanout_mode1_cdr",
          "devel/pc059_standalone_312_mhz-059_1_62-Registers.txt" }, // stand-alone mode
        { "pc059_5345_fanout_cdr",
          "devel/pc059_standalone_312_mhz-059_1_62-Registers.txt" }, // stand-alone mode

        { "fib_5395_fanout_mode0_cdr",
          "devel/Si5395-RevA-FIB_fanout-65_FA_31-Registers.txt" }, // fanout mode, data and clock from backplane
        { "fib_5395_fanout_mode1_cdr",
          "devel/Si5395-RevA-FIB_ouroboros-65_SA_31-Registers.txt" }, // stand-alone mode
        { "fib_5395_fanout_cdr",
          "devel/Si5395-RevA-FIB_ouroboros-65_SA_31-Registers.txt" }, // stand-alone mode  

        { "mib_5395_fanout_mode0_cdr",
          "devel/Si5395-RevA-MIB_62_1-Registers.txt" }, // fanout mode, data and clock always from upstream sfp 1
        { "mib_5395_fanout_mode1_cdr",
          "devel/Si5395-RevA-MIB_62_1-Registers.txt" }, // stand-alone mode

        { "fib_5395_ouroboros_cdr", "devel/Si5395-RevA-FIB_ouroboros-65_SA_31-Registers.txt" },
        { "fib_5395_ouroboros_cdr", "devel/Si5395-RevA-FIB_ouroboros-65_SA_31-Registers.txt" },

        // 50 MHz mappings
        { "fmc_5344_endpoint_50_mhz_cdr", "devel/ENDPOINT-Si5344-50MHzRef.txt"},
        { "fmc_5394_endpoint_50_mhz_cdr", "devel/ENDPOINT-Si5344-50MHzRef.txt"},

        { "fmc_5344_endpoint-bi-crt_50_mhz_cdr", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt" },
        { "fmc_5394_endpoint-bi-crt_50_mhz_cdr", "devel/Si5344-PDTSCRT1NoZdm-RevD-400HzBW-Registers.txt" },

        { "fmc_5344_chronos_50_mhz_cdr", "devel/ENDPOINT-Si5344-50MHzRef.txt" },
        { "fmc_5394_chronos_50_mhz_cdr", "devel/ENDPOINT-Si5344-50MHzRef.txt" },

        //{ "kFMCRev1_ouroboros_50_mhz", "SI5344/PDTS0000.txt" },
        { "fmc_5344_ouroboros_50_mhz_cdr", "SI5344/PDTS0003.txt" },
        { "fmc_5394_ouroboros_50_mhz_cdr", "SI5344/PDTS0003.txt" },

        //{ "kFMCRev1_master_50_mhz", "SI5344/PDTS0000.txt" },
        { "fmc_5344_master_50_mhz_cdr", "SI5344/PDTS0003.txt" },
        { "fmc_5394_master_50_mhz_cdr", "SI5344/PDTS0003.txt" },

        //{ "kFMCRev1_overlord_50_mhz", "SI5344/PDTS0000.txt" },
        { "fmc_5344_overlord_50_mhz_cdr", "SI5344/PDTS0003.txt" },
        { "fmc_5394_overlord_50_mhz_cdr", "SI5344/PDTS0003.txt" },

        //{ "kFMCRev1_boreas_50_mhz", "SI5344/PDTS0000.txt" },
        { "fmc_5344_boreas_50_mhz_cdr", "SI5344/PDTS0003.txt" },
        { "fmc_5394_boreas_50_mhz_cdr", "SI5344/PDTS0003.txt" },

        { "tlu_5345_overlord_50_mhz_cdr", "wr/TLU_EXTCLK_10MHZ_NOZDM.txt" },
        { "tlu_5345_boreas_50_mhz_cdr", "wr/TLU_EXTCLK_10MHZ_NOZDM.txt" },

        { "pc059_5345_ouroboros_50_mhz_cdr", "SI5345/PDTS0005.txt" },
        { "pc059_5345_fanout_mode0_50_mhz_cdr", "wr/FANOUT_PLL_WIDEBW_SFPIN.txt" }, // fanout mode
        { "pc059_5345_fanout_mode1_50_mhz_cdr", "devel/PDTS_PC059_FANOUT.txt" },    // stand-alone mode
        { "pc059_5345_fanout_50_mhz_cdr", "devel/PDTS_PC059_FANOUT.txt" },    // stand-alone mode
  };

  static inline const std::vector<BoardType> library_supported_boards = {
    kBoardFMC, kBoardPC059, kBoardTLU, kBoardSim, kBoardFIB, kBoardMIB, kBoardPC069, kBoardGIB
  };

  static inline const std::vector<DesignType> library_supported_designs = {
    kDesignMaster, kDesignOuroboros, kDesignOuroborosSim, kDesignEndpoint, kDesignFanout, kDesignOverlord, kDesignEndpoBICRT, kDesignChronos, kDesignBoreas, kDesignKerberos, kDesignGaia
  };

};

} // namespace timing
} // namespace dunedaq

#include "timing/detail/IONode.hxx"

#endif // TIMING_INCLUDE_TIMING_IONODE_HPP_