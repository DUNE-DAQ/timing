/**
 * @file HSINode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/HSINode.hpp"

#include "timing/FLCmdGeneratorNode.hpp"
#include "timing/toolbox.hpp"
#include "logging/Logging.hpp"

#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(HSINode)

//-----------------------------------------------------------------------------
HSINode::HSINode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
HSINode::~HSINode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
HSINode::get_status(bool print_out) const
{

  std::stringstream status;

  std::vector<std::pair<std::string, std::string>> ept_summary;
  std::vector<std::pair<std::string, std::string>> hsi_summary;

  // auto lEPTimestamp = getNode("tstamp").readBlock(2);

  auto hsi_control = read_sub_nodes(getNode("csr.ctrl"), false);
  auto hsi_state = read_sub_nodes(getNode("csr.stat"), false);

  auto hsi_buffer_count = getNode("buf.count").read();

  auto hsi_re_mask = getNode("csr.re_mask").read();
  auto hsi_fe_mask = getNode("csr.fe_mask").read();
  auto hsi_inv_mask = getNode("csr.inv_mask").read();

  getClient().dispatch();

  hsi_summary.push_back(std::make_pair("Source", format_reg_value(hsi_control.find("src")->second.value(), 16)));
  hsi_summary.push_back(std::make_pair("Enabled", format_reg_value(hsi_control.find("en")->second.value(), 16)));
  hsi_summary.push_back(std::make_pair("Rising edge mask", format_reg_value(hsi_re_mask.value(), 16)));
  hsi_summary.push_back(std::make_pair("Falling edge mask", format_reg_value(hsi_fe_mask.value(), 16)));
  hsi_summary.push_back(std::make_pair("Invert mask", format_reg_value(hsi_inv_mask.value(), 16)));
  hsi_summary.push_back(
    std::make_pair("Buffer enabled", format_reg_value(hsi_control.find("buf_en")->second.value(), 16)));
  hsi_summary.push_back(
    std::make_pair("Buffer error", format_reg_value(hsi_state.find("buf_err")->second.value(), 16)));
  hsi_summary.push_back(
    std::make_pair("Buffer warning", format_reg_value(hsi_state.find("buf_warn")->second.value(), 16)));
  hsi_summary.push_back(std::make_pair("Buffer occupancy", to_string(hsi_buffer_count.value())));

  status << format_reg_table(hsi_summary, "HSI summary", { "", "" }) << std::endl;

  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
HSINode::read_buffer_count() const
{
  auto buffer_count = getNode("buf.count").read();
  getClient().dispatch();
  return buffer_count.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uhal::ValVector<uint32_t>                                                             // NOLINT(build/unsigned)
HSINode::read_data_buffer(uint16_t& n_words, bool read_all, bool fail_on_error) const // NOLINT(build/unsigned)
{

  uint32_t buffer_state = read_buffer_state(); // NOLINT(build/unsigned)

  uint16_t n_hsi_words = buffer_state >> 0x10; // NOLINT(build/unsigned)

  n_words = n_hsi_words;

  TLOG_DEBUG(5) << "Words available in readout buffer:      " << format_reg_value(n_hsi_words);

  uhal::ValVector<uint32_t> buffer_data; // NOLINT(build/unsigned)

  if (buffer_state & 0x2) {
    ers::warning(HSIBufferIssue(ERS_HERE, "WARNING"));
  }

  if (buffer_state & 0x1) {
    ers::error(HSIBufferIssue(ERS_HERE, "ERROR"));
    if (fail_on_error)
      return buffer_data;
  }

  // this is bad
  if (n_hsi_words > 1024) {
    ers::error(HSIBufferIssue(ERS_HERE, "OVERFLOW"));
    if (fail_on_error)
      return buffer_data;
    n_hsi_words = 1024;
  }

  uint32_t events_to_read = n_hsi_words / hsi_buffer_event_words_number; // NOLINT(build/unsigned)

  TLOG_DEBUG(5) << "Events available in readout buffer:     " << format_reg_value(events_to_read);

  uint32_t words_to_read = read_all ? n_hsi_words : events_to_read * hsi_buffer_event_words_number; // NOLINT(build/unsigned)

  TLOG_DEBUG(5) << "Words to be read out in readout buffer: " << format_reg_value(words_to_read);

  if (!words_to_read) {
    TLOG_DEBUG(5) << "No words to be read out.";
  }

  buffer_data = getNode("buf.data").readBlock(words_to_read);
  getClient().dispatch();

  return buffer_data;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uhal::ValVector<uint32_t> // NOLINT(build/unsigned)
HSINode::read_data_buffer(bool read_all, bool fail_on_error) const
{
  uint16_t words; // NOLINT(build/unsigned)
  return read_data_buffer(words, read_all, fail_on_error);
}

//-----------------------------------------------------------------------------
std::string
HSINode::get_data_buffer_table(bool read_all, bool print_out) const
{

  std::stringstream table;
  auto buffer_data = read_data_buffer(read_all);

  std::vector<std::pair<std::string, uint32_t>> buffer_table; // NOLINT(build/unsigned)

  uint32_t i = 0; // NOLINT(build/unsigned)
  for (auto it = buffer_data.begin(); it != buffer_data.end(); ++it, ++i) {
    std::stringstream index_stream;
    index_stream << std::setfill('0') << std::setw(4) << i;
    buffer_table.push_back(std::make_pair(index_stream.str(), *it));
  }
  table << format_reg_table(buffer_table, "HSI buffer", { "Word", "Data" });

  if (print_out)
    TLOG() << table.str();
  return table.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::configure_hsi(uint32_t src,      // NOLINT(build/unsigned)
                       uint32_t re_mask,  // NOLINT(build/unsigned)
                       uint32_t fe_mask,  // NOLINT(build/unsigned)
                       uint32_t inv_mask, // NOLINT(build/unsigned)
                       double rate,
                       uint32_t clock_frequency_hz, // NOLINT(build/unsigned)
                       bool dispatch) const
{

  getNode("csr.ctrl.src").write(src);
  getNode("csr.re_mask").write(re_mask);
  getNode("csr.fe_mask").write(fe_mask);
  getNode("csr.inv_mask").write(inv_mask);

  // Configures the internal hsi signal generator to produce triggers at a defined frequency.
  // Rate =  (clock_frequency_hz / 2^(d+8)) / p where n in [0,15] and p in [1,256]

  // DIVIDER (int): Frequency divider.

  // The division from clock_frequency_hz to the desired rate is done in three steps:
  // a) A pre-division by 256
  // b) Division by a power of two set by n = 2 ^ rate_div_d (ranging from 2^0 -> 2^15)
  // c) 1-in-n prescaling set by n = rate_div_p
  
  try
  {
    uint32_t divisor;
    uint32_t prescale;
    double actual_rate;

    FLCmdGeneratorNode::parse_periodic_fl_cmd_rate(rate, clock_frequency_hz, actual_rate, divisor, prescale);

    TLOG() << "Requested rate, actual rate: " << rate << ", " << actual_rate;
    TLOG() << "prescale, divisor: " << prescale << ", " << divisor;

    std::stringstream trig_stream;
    trig_stream << "> Random trigger rate for HSI set to " << std::setprecision(3) << std::scientific << actual_rate << " Hz. d: " << divisor << " p: " << prescale;
    TLOG() << trig_stream.str();
    
    getNode("csr.ctrl.rate_div_p").write(prescale);
    getNode("csr.ctrl.rate_div_d").write(divisor);
  }
  catch (const timing::BadRequestedFakeTriggerRate& e)
  {
    ers::error(FailedToUpdateHSIRandomRate(ERS_HERE,e));
  }

  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::start_hsi(bool dispatch) const
{
  getNode("csr.ctrl.en").write(0x1);
  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::stop_hsi(bool dispatch) const
{
  getNode("csr.ctrl.en").write(0x0);
  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::reset_hsi(bool dispatch) const
{
  getNode("csr.ctrl.en").write(0x0);

  getNode("csr.ctrl.buf_en").write(0x0);
  getNode("csr.ctrl.buf_en").write(0x1);

  getNode("csr.re_mask").write(0x0);
  getNode("csr.fe_mask").write(0x0);
  getNode("csr.inv_mask").write(0x0);
  getNode("csr.ctrl.src").write(0x0);

  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
HSINode::read_buffer_warning() const
{
  auto buf_warning = getNode("csr.stat.buf_warn").read();
  getClient().dispatch();
  return buf_warning.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
HSINode::read_buffer_error() const
{
  auto buf_error = getNode("csr.stat.buf_err").read();
  getClient().dispatch();
  return buf_error.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
HSINode::read_buffer_state() const
{

  auto buf_state = read_sub_nodes(getNode("csr.stat"), false);
  auto hsi_buffer_count = getNode("buf.count").read();
  getClient().dispatch();

  uint8_t buffer_error = static_cast<uint8_t>(buf_state.find("buf_err")->second.value());    // NOLINT(build/unsigned)
  uint8_t buffer_warning = static_cast<uint8_t>(buf_state.find("buf_warn")->second.value()); // NOLINT(build/unsigned)

  uint32_t buffer_state = buffer_error | (buffer_warning << 1);                          // NOLINT(build/unsigned)
  buffer_state = buffer_state | static_cast<uint32_t>(hsi_buffer_count.value()) << 0x10; // NOLINT(build/unsigned)
  return buffer_state;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
HSINode::read_signal_source_mode() const
{
  auto source = getNode("csr.ctrl.src").read();
  getClient().dispatch();
  return source.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::get_info(timingfirmwareinfo::HSIFirmwareMonitorData& mon_data) const
{
  auto hsi_control = read_sub_nodes(getNode("csr.ctrl"), false);
  auto hsi_state = read_sub_nodes(getNode("csr.stat"), false);

  auto hsi_buffer_count = getNode("buf.count").read();

  auto hsi_re_mask = getNode("csr.re_mask").read();
  auto hsi_fe_mask = getNode("csr.fe_mask").read();
  auto hsi_inv_mask = getNode("csr.inv_mask").read();

  getClient().dispatch();

  mon_data.source = hsi_control.find("src")->second.value();
  mon_data.re_mask = hsi_re_mask.value();
  mon_data.fe_mask = hsi_fe_mask.value();
  mon_data.inv_mask = hsi_inv_mask.value();
  mon_data.buffer_enabled = hsi_control.find("buf_en")->second.value();
  mon_data.buffer_error = hsi_state.find("buf_err")->second.value();
  mon_data.buffer_warning = hsi_state.find("buf_warn")->second.value();
  mon_data.buffer_occupancy = hsi_buffer_count.value();
  mon_data.enabled = hsi_control.find("en")->second.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::get_info(opmonlib::InfoCollector& ci, int /*level*/) const
{
  timingfirmwareinfo::HSIFirmwareMonitorData mon_data;
  this->get_info(mon_data);
  ci.add(mon_data);
}
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq
