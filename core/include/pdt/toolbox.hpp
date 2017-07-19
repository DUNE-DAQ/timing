#ifndef __PDT_TOOLBOX_HPP__
#define __PDT_TOOLBOX_HPP__

// C++ Headers
#include <string>
#include <istream>
#include <stdint.h>
#include <stdlib.h>

// Boost Headers
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/unordered_map.hpp>

// uHAL Headers
#include <uhal/Node.hpp>

#include "definitions.hpp"

namespace pdt {

// Wrappers to be used by lexical_cast
template < typename T > struct stol;
template < typename T > struct stoul;

template < typename T >
std::string to_string(const T&);

template < typename M >
bool map_value_comparator( typename M::value_type &p1, typename M::value_type &p2);

template< typename T>
std::vector<T> sanitize( const std::vector<T>& vec ); 

template< typename T, typename U>
T safe_enum_cast(const U& value, const std::vector<T>& valid);

//! Walk & read the node structure.
Snapshot snapshot(const uhal::Node& aNode);

//! Walk & read the sub-nodes whose IDs match this regex.
Snapshot snapshot(const uhal::Node& aNode, const std::string& aRegex);

/**
 * Sleeps for a given number of milliseconds
 *
 * @param      aTimeInMilliseconds  Number of milliseconds to sleep
 */
void millisleep(const double& aTimeInMilliseconds);

/**
 * Formats a std::string in printf fashion
 *
 * @param[in]      aFmt   Format string
 * @param[in]      ...   List of arguments
 *
 * @return     A formatted string
 */
std::string strprintf(const char* aFmt, ...);

/**
 * Expand the path
 * @param aPath path to expand
 * @return vector of expanded paths
 */
std::vector<std::string> shellExpandPaths(const std::string& aPath);

/**
 * Performs variable subsitutition on path
 * @param aPath: Path to expand
 * @return Expanded path
 */
std::string shellExpandPath(const std::string& aPath);

/**
 * Checks that the input path corresponds to an existing filesystem item which 
 * is a file
 * @param aPath: Path to file
 */
void throwIfNotFile(const std::string& aPath);

/**
 * @brief      Converts a vector of strings in a delimiter-separated string.
 *
 * @param[in]  aStrings    Collection of strings
 * @param[in]  aDelimiter  A delimiter
 *
 * @tparam     C           { description }
 *
 * @return     { description_of_the_return_value }
 */
template<typename C>
std::string join(const C& aStrings, const std::string& aDelimiter = " ");

} // namespace pdt


#include "pdt/toolbox.hxx"

#endif /* __PDT_TOOLBOX_HPP__ */

