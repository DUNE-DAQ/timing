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

#include <pdt/python/converters.hpp>

using namespace boost::python;


namespace pdt {
namespace python {

extern void register_i2c();

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
	    
	    extract<dict>(getattr(import("sys"),"modules"))()["pdt.core"]=orbModule;
	    
	    // Change to sub-module scope ...
	    scope orbScope = orbModule;

		pdt::python::register_i2c();

	}
}