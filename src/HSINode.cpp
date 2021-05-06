#include "timing/HSINode.hpp"

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(HSINode)

//-----------------------------------------------------------------------------
HSINode::HSINode(const uhal::Node& node) : EndpointNode(node) {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
HSINode::~HSINode() {
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
HSINode::enable(uint32_t partition, uint32_t address) const {
	getNode("csr.ctrl.tgrp").write(partition);

	if (address) {
		//getNode("csr.ctrl.int_addr").write(0x1);
		getNode("csr.ctrl.addr").write(address);
	} else {
		//getNode("csr.ctrl.int_addr").write(0x0);
	}

	//getNode("csr.ctrl.ctr_rst").write(0x1);
	//getNode("csr.ctrl.ctr_rst").write(0x0);
	getNode("csr.ctrl.ep_en").write(0x1);
	//getNode("csr.ctrl.buf_en").write(0x1);
	getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
HSINode::disable() const {
	getNode("csr.ctrl.ep_en").write(0x0);
    //getNode("csr.ctrl.buf_en").write(0x0);
    getClient().dispatch();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
HSINode::reset(uint32_t partition, uint32_t address) const {

	getNode("csr.ctrl.ep_en").write(0x0);
	//getNode("csr.ctrl.buf_en").write(0x0);
	
	enable(partition, address);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
HSINode::get_status(bool print_out) const {
	
	std::stringstream lStatus;

	std::vector<std::pair<std::string, std::string> > lEPSummary;
	std::vector<std::pair<std::string, std::string> > hsi_summary;

	//auto lEPTimestamp = getNode("tstamp").readBlock(2);
	//auto lEPEventCounter = getNode("evtctr").read();
	//auto lEPBufferCount = getNode("buf.count").read();
	auto lEPControl = read_sub_nodes(getNode("csr.ctrl"), false);
	auto lEPState = read_sub_nodes(getNode("csr.stat"), false);
	//auto lEPCounters = getNode("ctrs").readBlock(g_command_number);
	
	auto hsi_control = read_sub_nodes(getNode("hsi.csr.ctrl"), false);
	auto hsi_state = read_sub_nodes(getNode("hsi.csr.stat"), false);

	auto hsi_buffer_count = getNode("hsi.buf.count").read();

	auto hsi_re_mask = getNode("hsi.csr.re_mask").read();
	auto hsi_fe_mask = getNode("hsi.csr.fe_mask").read();
	auto hsi_inv_mask = getNode("hsi.csr.inv_mask").read();

	getClient().dispatch();

	lEPSummary.push_back(std::make_pair("Enabled", std::to_string(lEPControl.find("ep_en")->second.value())));
	lEPSummary.push_back(std::make_pair("State", g_endpoint_state_map.at(lEPState.find("ep_stat")->second.value())));
	lEPSummary.push_back(std::make_pair("Partition", std::to_string(lEPControl.find("tgrp")->second.value())));
	lEPSummary.push_back(std::make_pair("Address", std::to_string(lEPControl.find("addr")->second.value())));
	//lEPSummary.push_back(std::make_pair("Timestamp", format_timestamp(lEPTimestamp)));
	//lEPSummary.push_back(std::make_pair("Timestamp (hex)", format_reg_value(tstamp2int(lEPTimestamp))));
	//lEPSummary.push_back(std::make_pair("EventCounter", std::to_string(lEPEventCounter.value())));
	//std::string lBufferStatusString = !lEPState.find("buf_err")->second.value() ? "OK" : "Error";
	//lEPSummary.push_back(std::make_pair("Buffer status", lBufferStatusString));
	//lEPSummary.push_back(std::make_pair("Buffer occupancy", std::to_string(lEPBufferCount.value())));

	//std::vector<std::pair<std::string, std::string> > lEPCommandCounters;

	//for (uint32_t i=0; i < g_command_number; ++i) {
	//	lEPCommandCounters.push_back(std::make_pair(g_command_map.at(i), std::to_string(lEPCounters[i])));	
	//}

	hsi_summary.push_back(std::make_pair("Buffer occupancy", std::to_string(hsi_buffer_count.value())));
	hsi_summary.push_back(std::make_pair("re_mask", std::to_string(hsi_re_mask.value())));
	hsi_summary.push_back(std::make_pair("fe_mask", std::to_string(hsi_fe_mask.value())));
	hsi_summary.push_back(std::make_pair("inv_mask", std::to_string(hsi_inv_mask.value())));
	
	lStatus << format_reg_table(lEPSummary, "Endpoint summary", {"", ""}) << std::endl;
	lStatus << format_reg_table(lEPState, "Endpoint state") << std::endl;
	
	lStatus << format_reg_table(hsi_control, "HSI control") << std::endl;
	lStatus << format_reg_table(hsi_state, "HSI state") << std::endl;

	lStatus << format_reg_table(hsi_summary, "HSI summary", {"", ""}) << std::endl;

	//lStatus << format_reg_table(lEPCommandCounters, "Endpoint counters", {"Command", "Counter"}); 

	if (print_out) std::cout << lStatus.str();
    return lStatus.str();       
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//uint64_t
//HSINode::read_timestamp() const {
//	auto lTimestamp = getNode("tstamp").readBlock(2);
//	getClient().dispatch();
//    return tstamp2int(lTimestamp);
//}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint32_t
HSINode::read_buffer_count() const {
	auto lBufCount = getNode("hsi.buf.count").read();
	getClient().dispatch();
	return lBufCount.value();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uhal::ValVector< uint32_t >
HSINode::read_data_buffer(bool read_all) const {
	
	auto lBufCount = getNode("hsi.buf.count").read();
	getClient().dispatch();

	TLOG_DEBUG(2) << "Words available in readout buffer:      " << format_reg_value(lBufCount);

	uint32_t lEventsToRead = lBufCount.value() / g_hsi_event_size;
	
	TLOG_DEBUG(2) << "Events available in readout buffer:     " << format_reg_value(lEventsToRead);

	uint32_t lWordsToRead = read_all ? lBufCount.value() : lEventsToRead * g_hsi_event_size;

	TLOG_DEBUG(2) << "Words to be read out in readout buffer: " << format_reg_value(lWordsToRead);
	
	if (!lWordsToRead) {
		TLOG_DEBUG(2) << "No words to be read out.";
	}
	
	auto lBufData = getNode("hsi.buf.data").readBlock(lWordsToRead);
	getClient().dispatch();

    return lBufData;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
HSINode::get_data_buffer_table(bool read_all, bool print_out) const {

	std::stringstream lTable;
	auto lBufData = read_data_buffer(read_all);

	std::vector<std::pair<std::string, uint32_t>> lBufferTable;

	uint32_t i=0;
	for (auto it=lBufData.begin(); it!=lBufData.end(); ++it, ++i) {
		std::stringstream lIndexStream;
		lIndexStream << std::setfill('0') << std::setw(4) << i;
		lBufferTable.push_back(std::make_pair(lIndexStream.str(), *it));
	}
	lTable << format_reg_table(lBufferTable, "HSI buffer", {"Word","Data"});
	
	if (print_out) std::cout << lTable.str();
	return lTable.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//double
//HSINode::read_clock_frequency() const {
//	std::vector<double> lFrequencies = getNode<FrequencyCounterNode>("freq").measure_frequencies(1);
//	return lFrequencies.at(0);
//}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//uint32_t
//HSINode::read_version() const {
//	auto lBufCount = getNode("version").read();
//	getClient().dispatch();
//	return lBufCount.value();
//}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//void
//HSINode::get_info(timingendpointinfo::TimingEndpointInfo& mon_data) const {
//	
//	auto timestamp = getNode("tstamp").readBlock(2);
//	auto event_counter = getNode("evtctr").read();
//	auto buffer_count = getNode("buf.count").read();
//	auto endpoint_control = read_sub_nodes(getNode("csr.ctrl"), false);
//	auto endpoint_state = read_sub_nodes(getNode("csr.stat"), false);
//	auto counters = getNode("ctrs").readBlock(g_command_number);
//	getClient().dispatch();
//	
//	mon_data.state = endpoint_state.at("ep_stat").value();
//	mon_data.ready = endpoint_state.at("ep_rdy").value();
//	mon_data.partition = endpoint_control.at("tgrp").value();
//	mon_data.address = endpoint_control.at("addr").value();
//	mon_data.timestamp = tstamp2int(timestamp);
//	mon_data.in_run = endpoint_state.at("in_run").value();
//	mon_data.in_spill = endpoint_state.at("in_spill").value();
//	mon_data.buffer_warning = endpoint_state.at("buf_warn").value();
//	mon_data.buffer_error = endpoint_state.at("buf_err").value();
//	mon_data.buffer_occupancy = buffer_count.value();
//	mon_data.event_counter = event_counter.value();
//    mon_data.reset_out = endpoint_state.at("ep_rsto").value();
//	mon_data.sfp_tx_disable = endpoint_state.at("sfp_tx_dis").value();
//	mon_data.coarse_delay = endpoint_state.at("cdelay").value();
//	mon_data.fine_delay = endpoint_state.at("fdelay").value();
//
//	//for (uint32_t i=0; i < g_command_number; ++i) {
//	//	counters.push_back(std::make_pair(g_command_map.at(i), std::to_string(counters[i])));	
//	//}      
//}
//-----------------------------------------------------------------------------

void
HSINode::start_hsi(uint32_t src, uint32_t re_mask, uint32_t fe_mask, uint32_t inv_mask, bool dispatch) const {

	getNode("hsi.csr.ctrl.src").write(src);
	getNode("hsi.csr.re_mask").write(re_mask);
	getNode("hsi.csr.fe_mask").write(fe_mask);
	getNode("hsi.csr.inv_mask").write(inv_mask);

	getNode("hsi.csr.ctrl.en").write(0x1);

	if (dispatch) getClient().dispatch();
}

void
HSINode::stop_hsi(bool dispatch) const {
	getNode("hsi.csr.ctrl.en").write(0x0);
	if (dispatch) getClient().dispatch();
}

void
HSINode::reset_hsi(bool dispatch) const {
	getNode("hsi.csr.ctrl.en").write(0x0);
	
	getNode("hsi.csr.ctrl.buf_en").write(0x0);
	getNode("hsi.csr.ctrl.buf_en").write(0x1);

	getNode("hsi.csr.re_mask").write(0x0);
	getNode("hsi.csr.fe_mask").write(0x0);
	getNode("hsi.csr.inv_mask").write(0x0);
	getNode("hsi.csr.ctrl.src").write(0x0);

	if (dispatch) getClient().dispatch();
}

bool
HSINode::read_buffer_warning() const {
	auto buf_warning = getNode("hsi.csr.stat.buf_warn").read();
	getClient().dispatch();
	return buf_warning.value();
}


bool
HSINode::read_buffer_error() const {
	auto buf_error = getNode("hsi.csr.stat.buf_err").read();
	getClient().dispatch();
	return buf_error.value();
}


} // namespace timing
} // namespace dunedaq