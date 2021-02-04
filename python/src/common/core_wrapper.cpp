#include <boost/python/module.hpp>

#include <boost/python/def.hpp>
#include <boost/python/wrapper.hpp>
#include <boost/python/overloads.hpp>
#include <boost/python/class.hpp>
#include <boost/python/enum.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/import.hpp>
#include <boost/python/copy_const_reference.hpp>

#include "pdt/python/converters.hpp"


using namespace boost::python;


namespace pdt {
namespace python {

extern void register_i2c();
extern void register_partition();
extern void register_endpoint();
extern void register_io();
extern void register_master();
extern void register_top_designs();

} // namespace python
} // namespace pdt

BOOST_PYTHON_MODULE(_pdt) {

	pdt::python::register_converters();

	{
		scope packageScope;
		std::string orbModuleName(extract<const char*> (packageScope.attr("__name__")));
		orbModuleName += ".core";
		char* orbModuleCstr = new char [orbModuleName.size() + 1];
		strcpy(orbModuleCstr, orbModuleName.c_str());

		// Make test sub-module ...
		object orbModule(handle<> (borrowed(PyImport_AddModule(orbModuleCstr)))); //< Enables "from pdt.core import <whatever>"
		orbModule.attr("__file__") = "<synthetic>";

		packageScope.attr("core") = orbModule; //< Enables "from pdt import core"

		extract<dict>(getattr(import("sys"), "modules"))()["pdt.core"] = orbModule;

		// Change to sub-module scope ...
		scope orbScope = orbModule;

		//enum_<pdt::LogLevel> ("LogLevel")
		//.value("kDebug4", pdt::kDebug4)
		//.value("kDebug3", pdt::kDebug3)
		//.value("kDebug2", pdt::kDebug2)
		//.value("kDebug1", pdt::kDebug1)
		//.value("kDebug", pdt::kDebug)
		//.value("kInfo", pdt::kInfo)
		//.value("kNotice", pdt::kNotice)
		//.value("kWarning", pdt::kWarning)
		//.value("kError", pdt::kError)
		//.export_values()
		//;

		//def("setLogThreshold", &pdt::Log::setLogThreshold);



		pdt::python::register_i2c();
		pdt::python::register_partition();
		pdt::python::register_endpoint();
		pdt::python::register_io();
		pdt::python::register_master();
		pdt::python::register_top_designs();

	}
}