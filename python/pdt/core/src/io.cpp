#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "pdt/IONode.hpp"
#include "pdt/FMCIONode.hpp"
#include "pdt/PC059IONode.hpp"
#include "pdt/TLUIONode.hpp"
#include "pdt/SIMIONode.hpp"

namespace py = pybind11;

namespace dunedaq {
namespace pdt {
namespace python {

void
register_io(py::module& m) {
      
      py::class_<pdt::IONode, uhal::Node> (m, "IONode")
      ;
      
      py::class_<pdt::FMCIONode, pdt::IONode, uhal::Node> (m, "FMCIONode")
        .def(py::init<const uhal::Node&>())
        .def("reset", &pdt::FMCIONode::reset, py::arg("aClockConfigFile") = "")
        .def("softReset", &pdt::FMCIONode::softReset)
        .def("getClockFrequenciesTable", &pdt::FMCIONode::getClockFrequenciesTable, py::arg("aPrint") = false)
        .def("get_status", &pdt::FMCIONode::get_status, py::arg("aPrint") = false)
        .def("getPLLStatus", &pdt::FMCIONode::getPLLStatus, py::arg("aPrint") = false)
        .def("getHardwareInfo", &pdt::FMCIONode::getHardwareInfo, py::arg("aPrint") = false)
        .def("getSFPStatus", &pdt::FMCIONode::getSFPStatus, py::arg("aSFPId"), py::arg("aPrint") = false)
        .def("switchSFPSoftTxControlBit", &pdt::FMCIONode::switchSFPSoftTxControlBit)
      ;

      py::class_<pdt::PC059IONode, pdt::IONode, uhal::Node> (m, "PC059IONode")
        .def(py::init<const uhal::Node&>())
        .def<void (pdt::PC059IONode::*)(const std::string&) const>("reset", &pdt::PC059IONode::reset, py::arg("aClockConfigFile") = "")
        .def<void (pdt::PC059IONode::*)(int32_t,const std::string&) const>("reset", &pdt::PC059IONode::reset, py::arg("aFanoutMode"), py::arg("aClockConfigFile") = "")
        .def("softReset", &pdt::PC059IONode::softReset)
        .def("getClockFrequenciesTable", &pdt::PC059IONode::getClockFrequenciesTable, py::arg("aPrint") = false)
        .def("get_status", &pdt::PC059IONode::get_status, py::arg("aPrint") = false)
        .def("getPLLStatus", &pdt::PC059IONode::getPLLStatus, py::arg("aPrint") = false)
        .def("getHardwareInfo", &pdt::PC059IONode::getHardwareInfo, py::arg("aPrint") = false)
        .def("getSFPStatus", &pdt::PC059IONode::getSFPStatus, py::arg("aSFPId"), py::arg("aPrint") = false)
        .def("switchSFPSoftTxControlBit", &pdt::PC059IONode::switchSFPSoftTxControlBit)
        .def("switchSFPMUXChannel", &pdt::PC059IONode::switchSFPMUXChannel)
        .def("readActiveSFPMUXChannel", &pdt::PC059IONode::readActiveSFPMUXChannel)
      ;

      py::class_<pdt::TLUIONode, pdt::IONode, uhal::Node> (m, "TLUIONode")
        .def(py::init<const uhal::Node&>())
        .def("reset", &pdt::TLUIONode::reset, py::arg("aClockConfigFile") = "")
        .def("softReset", &pdt::TLUIONode::softReset)
        .def("getClockFrequenciesTable", &pdt::TLUIONode::getClockFrequenciesTable, py::arg("aPrint") = false)
        .def("get_status", &pdt::TLUIONode::get_status, py::arg("aPrint") = false)
        .def("getPLLStatus", &pdt::TLUIONode::getPLLStatus, py::arg("aPrint") = false)
        .def("getHardwareInfo", &pdt::TLUIONode::getHardwareInfo, py::arg("aPrint") = false)
        .def("getSFPStatus", &pdt::TLUIONode::getSFPStatus, py::arg("aSFPId"), py::arg("aPrint") = false)
        .def("switchSFPSoftTxControlBit", &pdt::TLUIONode::switchSFPSoftTxControlBit)
        .def("configureDAC", &pdt::TLUIONode::configureDAC, py::arg("aDACId"), py::arg("aDACValue"), py::arg("aInternalRef") = false)
        ;
  
      py::class_<pdt::SIMIONode, pdt::IONode, uhal::Node> (m, "SIMIONode")
        .def(py::init<const uhal::Node&>())
        .def("reset", &pdt::SIMIONode::reset, py::arg("aClockConfigFile") = "")
        .def("softReset", &pdt::SIMIONode::softReset)
        .def("getClockFrequenciesTable", &pdt::SIMIONode::getClockFrequenciesTable, py::arg("aPrint") = false)
        .def("get_status", &pdt::SIMIONode::get_status, py::arg("aPrint") = false)
        .def("getPLLStatus", &pdt::SIMIONode::getPLLStatus, py::arg("aPrint") = false)
        .def("getHardwareInfo", &pdt::SIMIONode::getHardwareInfo, py::arg("aPrint") = false)
        .def("getSFPStatus", &pdt::SIMIONode::getSFPStatus, py::arg("aSFPId"), py::arg("aPrint") = false)
        .def("switchSFPSoftTxControlBit", &pdt::SIMIONode::switchSFPSoftTxControlBit)
        ;

}

} // namespace python
} // namespace pdt
} // namespace dunedaq