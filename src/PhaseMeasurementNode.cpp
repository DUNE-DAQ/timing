/**
 * @file EchoMonitorNode.cpp
 *
 * This is part of the DUNE DAQ Software Suite, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "timing/PhaseMeasurementNode.hpp"

#include "timing/TimingIssues.hpp"
#include "timing/toolbox.hpp"
#include "logging/Logging.hpp"

#include <string>
#include <chrono>
#include <cmath>

namespace dunedaq {
namespace timing {

UHAL_REGISTER_DERIVED_NODE(PhaseMeasurementNode)

//-----------------------------------------------------------------------------
PhaseMeasurementNode::PhaseMeasurementNode(const uhal::Node& node)
  : TimingNode(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
PhaseMeasurementNode::~PhaseMeasurementNode() {}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::string
PhaseMeasurementNode::get_status(bool print_out) const
{
  std::stringstream status;
  auto subnodes = read_sub_nodes(getNode("stat"));
  status << format_reg_table(subnodes, "Phase measurement state");
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
PhaseMeasurementNode::reset() const
{
  getNode("csr.ctrl.rst").write(0x1);
  getNode("csr.ctrl.rst").write(0x0);
  getClient().dispatch();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
std::map<uint,double>
PhaseMeasurementNode::measure_phases() const
{
    reset();

    getNode("csr.ctrl.go").write(0x1);
    getNode("csr.ctrl.go").write(0x0);
    getClient().dispatch();
    
    uint timeout = 1000;
    auto start = std::chrono::high_resolution_clock::now();

    // Wait for the rx and phase to be happy
    while (true)
    {
        auto measurement_valid = getNode("csr.stat.valid").read();
        auto los_flag = getNode("csr.stat.los").read();
        getClient().dispatch();

        TLOG_DEBUG(7) << std::hex << "phase measurement valid: " << measurement_valid.value() << ", los flag: " << los_flag.value();

        if (measurement_valid.value())
        {
            TLOG_DEBUG(4) << "Phase measurement ready!";
            break;
        }

        auto now = std::chrono::high_resolution_clock::now();
        auto ms_since_start = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

        if (ms_since_start.count() > timeout)
            throw PhaseMeasurementNotReady(ERS_HERE, los_flag.value(), measurement_valid.value());

        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    double phase_ref0 = calculate_phase(0);
    double phase_ref1 = calculate_phase(1);

    std::map<uint,double> phases({{0,phase_ref0}, {1, phase_ref1}});
    
    return phases;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
PhaseMeasurementNode::measure_phase(uint8_t reference_clock) const
{
    auto phases = measure_phases();

    double selected_phase=-1;
    if (reference_clock==255)
    {
        if (phases[0] < 1 || phases[0] > 15)
        {
            selected_phase=phases[1];
        }
        else
        {
            selected_phase=phases[0];
        }
    }
    else
    {
        if (phases.count(reference_clock))
        {
            selected_phase = phases[reference_clock];
        }
        else
        {
            // todo error throw something
            throw;
        }
    }
    
    return selected_phase;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
double
PhaseMeasurementNode::calculate_phase(uint reference_clock) const
{   
    double counts_total = 67108864; // TODO read from fw

    std::string reg_label = "clk"+std::to_string(reference_clock);
    auto early = getNode("csr.stat."+reg_label+"_early").read();
    auto xor_counts_reg = getNode("csr.xor_counts_"+reg_label).read();
    getClient().dispatch();

    double xor_counts = xor_counts_reg.value();

    TLOG_DEBUG(6) << "xor : " << xor_counts <<  " tot : " << (uint64_t)counts_total;        
    double phase = 16*(xor_counts/counts_total);

    if (early)
    {
        phase = 16-phase;
    }

    // correct for the nominal phase differnce between master and reference clock, need to correct for clk0/clk1 offset from fw via vivado tools
    // TODO look up parameters and make generic call below
    if (reference_clock == 1)
    {
        phase = std::fmod(phase+2,16);
    }
    return phase;
}
//-----------------------------------------------------------------------------

} // namespace timing
} // namespace dunedaq