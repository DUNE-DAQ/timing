/**
 * @file HSINode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/HSINode.hpp"

#include "logging/Logging.hpp"

#include <string>
#include <utility>
#include <vector>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(HSINode)

//-----------------------------------------------------------------------------
HSINode::HSINode(const uhal::Node& node)
  : EndpointNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
HSINode::~HSINode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::enable(uint32_t partition, uint32_t address) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.tgrp").write(partition);
  getNode("csr.ctrl.addr").write(address);
  getNode("csr.ctrl.ep_en").write(0x1);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::disable() const
{
  getNode("csr.ctrl.ep_en").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::reset(uint32_t partition, uint32_t address) const // NOLINT(build/unsigned)
{
  getNode("csr.ctrl.ep_en").write(0x0);
  enable(partition, address);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
HSINode::get_status(bool print_out) const
{

  std::stringstream lStatus;

  std::vector<std::pair<std::string, std::string>> ept_summary;
  std::vector<std::pair<std::string, std::string>> hsi_summary;

  // auto lEPTimestamp = getNode("tstamp").readBlock(2);
  auto ept_control = read_sub_nodes(getNode("csr.ctrl"), false);
  auto ept_state = read_sub_nodes(getNode("csr.stat"), false);

  auto hsi_control = read_sub_nodes(getNode("hsi.csr.ctrl"), false);
  auto hsi_state = read_sub_nodes(getNode("hsi.csr.stat"), false);

  auto hsi_buffer_count = getNode("hsi.buf.count").read();

  auto hsi_re_mask = getNode("hsi.csr.re_mask").read();
  auto hsi_fe_mask = getNode("hsi.csr.fe_mask").read();
  auto hsi_inv_mask = getNode("hsi.csr.inv_mask").read();

  getClient().dispatch();

  ept_summary.push_back(std::make_pair("Enabled", format_reg_value(ept_control.find("ep_en")->second.value(), 16)));
  ept_summary.push_back(std::make_pair("Partition", format_reg_value(ept_control.find("tgrp")->second.value(), 16)));
  ept_summary.push_back(std::make_pair("Address", format_reg_value(ept_control.find("addr")->second.value(), 16)));
  ept_summary.push_back(std::make_pair("State", g_endpoint_state_map.at(ept_state.find("ep_stat")->second.value())));

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

  lStatus << format_reg_table(ept_summary, "Endpoint summary", { "", "" }) << std::endl;
  lStatus << format_reg_table(hsi_summary, "HSI summary", { "", "" }) << std::endl;

  if (print_out)
    TLOG() << lStatus.str();
  return lStatus.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
HSINode::read_buffer_count() const
{
  auto lBufCount = getNode("hsi.buf.count").read();
  getClient().dispatch();
  return lBufCount.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uhal::ValVector<uint32_t>                                                             // NOLINT(build/unsigned)
HSINode::read_data_buffer(uint16_t& n_words, bool read_all, bool fail_on_error) const // NOLINT(build/unsigned)
{

  uint32_t buffer_state = read_buffer_state(); // NOLINT(build/unsigned)

  uint16_t n_hsi_words = buffer_state >> 0x10; // NOLINT(build/unsigned)

  n_words = n_hsi_words;

  TLOG_DEBUG(2) << "Words available in readout buffer:      " << format_reg_value(n_hsi_words);

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

  uint32_t lEventsToRead = n_hsi_words / g_hsi_event_size; // NOLINT(build/unsigned)

  TLOG_DEBUG(2) << "Events available in readout buffer:     " << format_reg_value(lEventsToRead);

  uint32_t lWordsToRead = read_all ? n_hsi_words : lEventsToRead * g_hsi_event_size; // NOLINT(build/unsigned)

  TLOG_DEBUG(2) << "Words to be read out in readout buffer: " << format_reg_value(lWordsToRead);

  if (!lWordsToRead) {
    TLOG_DEBUG(2) << "No words to be read out.";
  }

  buffer_data = getNode("hsi.buf.data").readBlock(lWordsToRead);
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

  std::stringstream lTable;
  auto lBufData = read_data_buffer(read_all);

  std::vector<std::pair<std::string, uint32_t>> lBufferTable; // NOLINT(build/unsigned)

  uint32_t i = 0; // NOLINT(build/unsigned)
  for (auto it = lBufData.begin(); it != lBufData.end(); ++it, ++i) {
    std::stringstream lIndexStream;
    lIndexStream << std::setfill('0') << std::setw(4) << i;
    lBufferTable.push_back(std::make_pair(lIndexStream.str(), *it));
  }
  lTable << format_reg_table(lBufferTable, "HSI buffer", { "Word", "Data" });

  if (print_out)
    TLOG() << lTable.str();
  return lTable.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::configure_hsi(uint32_t src,      // NOLINT(build/unsigned)
                       uint32_t re_mask,  // NOLINT(build/unsigned)
                       uint32_t fe_mask,  // NOLINT(build/unsigned)
                       uint32_t inv_mask, // NOLINT(build/unsigned)
                       bool dispatch) const
{

  getNode("hsi.csr.ctrl.src").write(src);
  getNode("hsi.csr.re_mask").write(re_mask);
  getNode("hsi.csr.fe_mask").write(fe_mask);
  getNode("hsi.csr.inv_mask").write(inv_mask);

  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::start_hsi(bool dispatch) const
{
  getNode("hsi.csr.ctrl.en").write(0x1);
  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::stop_hsi(bool dispatch) const
{
  getNode("hsi.csr.ctrl.en").write(0x0);
  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::reset_hsi(bool dispatch) const
{
  getNode("hsi.csr.ctrl.en").write(0x0);

  getNode("hsi.csr.ctrl.buf_en").write(0x0);
  getNode("hsi.csr.ctrl.buf_en").write(0x1);

  getNode("hsi.csr.re_mask").write(0x0);
  getNode("hsi.csr.fe_mask").write(0x0);
  getNode("hsi.csr.inv_mask").write(0x0);
  getNode("hsi.csr.ctrl.src").write(0x0);

  if (dispatch)
    getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
HSINode::read_buffer_warning() const
{
  auto buf_warning = getNode("hsi.csr.stat.buf_warn").read();
  getClient().dispatch();
  return buf_warning.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool
HSINode::read_buffer_error() const
{
  auto buf_error = getNode("hsi.csr.stat.buf_err").read();
  getClient().dispatch();
  return buf_error.value();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint32_t // NOLINT(build/unsigned)
HSINode::read_buffer_state() const
{

  auto buf_state = read_sub_nodes(getNode("hsi.csr.stat"), false);
  auto hsi_buffer_count = getNode("hsi.buf.count").read();
  getClient().dispatch();

  uint8_t buffer_error = static_cast<uint8_t>(buf_state.find("buf_err")->second.value());    // NOLINT(build/unsigned)
  uint8_t buffer_warning = static_cast<uint8_t>(buf_state.find("buf_warn")->second.value()); // NOLINT(build/unsigned)

  uint32_t buffer_state = buffer_error | (buffer_warning << 1);                          // NOLINT(build/unsigned)
  buffer_state = buffer_state | static_cast<uint32_t>(hsi_buffer_count.value()) << 0x10; // NOLINT(build/unsigned)
  return buffer_state;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
HSINode::get_info(timingfirmwareinfo::HSIFirmwareMonitorData& mon_data) const
{

  auto ept_control = read_sub_nodes(getNode("csr.ctrl"), false);
  auto ept_state = read_sub_nodes(getNode("csr.stat"), false);

  auto hsi_control = read_sub_nodes(getNode("hsi.csr.ctrl"), false);
  auto hsi_state = read_sub_nodes(getNode("hsi.csr.stat"), false);

  auto hsi_buffer_count = getNode("hsi.buf.count").read();

  auto hsi_re_mask = getNode("hsi.csr.re_mask").read();
  auto hsi_fe_mask = getNode("hsi.csr.fe_mask").read();
  auto hsi_inv_mask = getNode("hsi.csr.inv_mask").read();

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

  mon_data.endpoint_enabled = ept_control.find("ep_en")->second.value();
  mon_data.endpoint_address = ept_control.find("addr")->second.value();
  mon_data.endpoint_partition = ept_control.find("tgrp")->second.value();
  mon_data.endpoint_state = ept_state.find("ep_stat")->second.value();
}
//-----------------------------------------------------------------------------
} // namespace timing
} // namespace dunedaq