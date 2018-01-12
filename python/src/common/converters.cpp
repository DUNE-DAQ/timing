#include "pdt/python/converters.hpp"

#include <boost/python/to_python_converter.hpp>
#include <boost/python/scope.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/converter/registry.hpp>

// C++ Headers
#include <utility>

// MP7 Headers

namespace bpy = boost::python;

namespace pdt {
namespace python {

void
register_converters() {
    /*
     * Register the following converters:
     *
     * list <=> std::vector<bool>
     *
     * list <=> std::vector<uint64_t>
     *
     * tuple <=> std::pair<uit32_t, bool>
     *
     * list <=> std::vector< std::pair<uit32_t, bool> >
     *
     * std::map<std::string, uint32_t> => dict
     *
     * boost::container::map<std::string, std::string> => dict
     *
     * list <=> std::vector<mp7::ValidData>
     *
     * Already registered by uhal
     *
     * list <=> std::vector<uint32_t>
     *
     */
    
    // The following converters may be already registered.
    // therefore they must be protected.

    const bpy::converter::registration* reg;

    // info = bpy::type_id< std::vector<bool> >(); 
    reg = bpy::converter::registry::query(bpy::type_id< std::vector<bool> >());
    if (reg == NULL or (*reg).m_to_python == NULL) {
        Converter_std_vector_from_list<bool>();
        bpy::to_python_converter < std::vector<bool>, Converter_std_vector_to_list<bool> >();
    }

    // std::vector<uint8_t>
    reg = bpy::converter::registry::query(bpy::type_id< std::vector<uint8_t> >());
    if (reg == NULL or (*reg).m_to_python == NULL) {
    // Duplicated converter
        Converter_std_vector_from_list<uint8_t>();
        bpy::to_python_converter< std::vector<uint8_t>, Converter_std_vector_to_list<uint8_t> >();
    }

    // // std::vector<uint32_t>
    // reg = bpy::converter::registry::query(bpy::type_id< std::vector<uint32_t> >());
    // if (reg == NULL or (*reg).m_to_python == NULL) {
    // // Duplicated converter
    //     Converter_std_vector_from_list<uint32_t>();
    //     bpy::to_python_converter< std::vector<uint32_t>, Converter_std_vector_to_list<uint32_t> >();
    // }   


    // std::vector<uint64_t>
    reg = bpy::converter::registry::query(bpy::type_id< std::vector<uint64_t> >());
    if (reg == NULL or (*reg).m_to_python == NULL) {
    // Duplicated converter
        Converter_std_vector_from_list<uint64_t>();
        bpy::to_python_converter< std::vector<uint64_t>, Converter_std_vector_to_list<uint64_t> >();
    }   

    // // std::map<std::string, uint32_t>
    // reg = bpy::converter::registry::query(bpy::type_id< std::map<std::string, uint32_t> >());
    // if (reg == NULL or (*reg).m_to_python == NULL) {
    //     Converter_std_map_from_dict<std::string, uint32_t>();
    //     bpy::to_python_converter< std::map<std::string, uint32_t>, pycomp7::Converter_std_map_to_dict<std::string, uint32_t> >();
    // }

    // // std::map<std::string, uint32_t>
    // reg = bpy::converter::registry::query(bpy::type_id< std::map<std::string, std::string> >());
    // if (reg == NULL or (*reg).m_to_python == NULL) {
    //     Converter_std_map_from_dict<std::string, std::string>();
    //     bpy::to_python_converter< std::map<std::string, std::string>, pycomp7::Converter_std_map_to_dict<std::string, std::string> >();
    // }

    // // std::map<uint32_t, uint32_t>
    // reg = bpy::converter::registry::query(bpy::type_id< std::map<uint32_t, uint32_t> >());
    // if (reg == NULL or (*reg).m_to_python == NULL) {
    //     Converter_std_map_from_dict<uint32_t, uint32_t>();
    //     bpy::to_python_converter< std::map<uint32_t, uint32_t>, pycomp7::Converter_std_map_to_dict<uint32_t, uint32_t> >();
    // }

    // // std::map<uint32_t, std::vector<uint32_t> >
    // reg = bpy::converter::registry::query(  bpy::type_id< std::map<uint32_t, std::vector<uint32_t> > >() );
    // if (reg == NULL or (*reg).m_to_python == NULL) {
    //     bpy::to_python_converter< std::map<uint32_t, std::vector<uint32_t> >, pycomp7::Converter_std_map_to_dict<uint32_t, std::vector<uint32_t> > >();    
    // }


}

} // namespace python
} // namespace pdt

