#ifndef TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOOLBOX_HPP_
#define TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOOLBOX_HPP_

#include "definitions.hpp"

#include "TimingIssues.hpp"

#include "ers/ers.h"

// uHAL Headers
#include <uhal/Node.hpp>

// Boost Headers
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/unordered_map.hpp>
#include <boost/format.hpp>
#include <boost/format/group.hpp>

// C++ Headers
#include <string>
#include <istream>
#include <iomanip>
#include <stdint.h>
#include <stdlib.h>
#include <chrono>

namespace dunedaq {
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
std::vector<std::string> shell_expand_paths(const std::string& aPath);

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
void throw_if_not_file(const std::string& aPath);

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

/**
 * Expand the path
 * @param aPath path to expand
 * @return vector of expanded paths
 */
uint8_t dec_rng(uint8_t word, uint8_t ibit, uint8_t nbits=1);

/**
 * ""
 * @return 
 */
uint64_t tstamp2int(uhal::ValVector< uint32_t > rawTimestamp);

/**
 * Format reg-value
 * @return 
 */
template <class T>
std::string format_reg_value(T regValue, uint32_t base=16);

/**
 * @brief     Format reg-value table
 * @return 
 */
template<class T>
std::string format_reg_table(T data, std::string title="", std::vector<std::string> headers={"Register", "Value"});

/**
 * ""
 * @return 
 */
int64_t get_seconds_since_epoch();

/**
 * ""
 * @return 
 */
std::string format_timestamp(uhal::ValVector< uint32_t > rawTimestamp);

/**
 * ""
 * @return 
 */
std::string format_timestamp(uint64_t rawTimestamp);


/**
 * @brief     Format reg-value table
 * @return 
 */
template<class T>
std::string format_counters_table(std::vector<T> aCounters, std::vector<std::string> aCounterNodeTitles={}, std::string aTableTitle="", std::vector<std::string> aCounterLabels={}, std::string aCounterLabelsHeader="Cmd");

/**
 * ""
 * @return 
 */
double convert_bits_to_float(uint64_t aBits, bool aIsDoublePrecision=false);


BoardType convert_value_to_board_type(uint32_t aBoardType);
CarrierType convert_value_to_carrier_type(uint32_t aCarrierType);
DesignType convert_value_to_design_type(uint32_t aDesignType);

template<typename T>
std::string vec_fmt(const std::vector<T>& aVec);

template<typename T>
std::string short_vec_fmt(const std::vector<T>& aVec);


} // namespace pdt
} // namespace dunedaq

#include "detail/toolbox.hxx"

#endif // TIMING_BOARD_SOFTWARE_INCLUDE_PDT_TOOLBOX_HPP_

