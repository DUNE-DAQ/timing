#include "pdt/toolbox.hpp"

// C++ Headers
#include <time.h>
#include <cstdarg>
#include <cstdlib>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <wordexp.h>
#include <stdexcept>

// Boost Headers
#include "boost/foreach.hpp"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// uHAL Headers
#include "uhal/ValMem.hpp"

// PDT Headers
#include "pdt/TimingIssues.hpp"

using namespace std;

namespace dunedaq {
namespace pdt {

//-----------------------------------------------------------------------------
Snapshot
snapshot(const uhal::Node& node) {
    /// snapshot( node ) -> { subnode:value }
    std::map<string, uhal::ValWord<uint32_t> > valWords;

    for(string n :  node.getNodes()) {
        valWords.insert(make_pair(n, node.getNode(n).read()));
    }
    node.getClient().dispatch();

    Snapshot vals;
    std::map<string, uhal::ValWord<uint32_t> >::iterator it;
    for (it = valWords.begin(); it != valWords.end(); ++it)
        vals.insert(make_pair(it->first, it->second.value()));

    return vals;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
Snapshot
snapshot(const uhal::Node& node, const std::string& aRegex) {
    std::map<string, uhal::ValWord<uint32_t> > valWords;

    for(string n :  node.getNodes(aRegex)) {
        valWords.insert(make_pair(n, node.getNode(n).read()));
    }
    node.getClient().dispatch();

    Snapshot vals;
    std::map<string, uhal::ValWord<uint32_t> >::iterator it;
    for (it = valWords.begin(); it != valWords.end(); ++it)
        vals.insert(make_pair(it->first, it->second.value()));

    return vals;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
millisleep(const double& aTimeInMilliseconds) {
    //  using namespace uhal;
    //  logging();
    double lTimeInSeconds(aTimeInMilliseconds / 1e3);
    int lIntegerPart((int) lTimeInSeconds);
    double lFractionalPart(lTimeInSeconds - (double) lIntegerPart);
    struct timespec sleepTime, returnTime;
    sleepTime.tv_sec = lIntegerPart;
    sleepTime.tv_nsec = (long) (lFractionalPart * 1e9);
    nanosleep(&sleepTime, &returnTime);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
strprintf(const char* fmt, ...) {
    char* ret;
    va_list ap;
    va_start(ap, fmt);
    vasprintf(&ret, fmt, ap);
    va_end(ap);
    std::string str(ret);
    free(ret);
    return str;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::vector<std::string>
shell_expand_paths(const std::string& aPath) {

    std::vector<std::string> lPaths;
    wordexp_t lSubstitutedPath;
    int code = wordexp(aPath.c_str(), &lSubstitutedPath, WRDE_NOCMD);
    if (code) throw runtime_error("Failed expanding path: " + aPath);

    for (std::size_t i = 0; i != lSubstitutedPath.we_wordc; i++)
        lPaths.push_back(lSubstitutedPath.we_wordv[i]);

    wordfree(&lSubstitutedPath);

    return lPaths;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string
shellExpandPath(const std::string& aPath) {
    std::vector<std::string> lPaths = shell_expand_paths(aPath);

    if (lPaths.size() > 1) throw runtime_error("Failed to expand: multiple matches found");

    return lPaths[0];
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void
throw_if_not_file(const std::string& aPath) {

    // FIXME: Review the implementation. The function never returns 
    namespace fs = boost::filesystem;

    // Check that the path exists and that it's not a directory
    fs::path cfgFile(aPath);
    if (!fs::exists(cfgFile)) {
        throw FileNotFound(ERS_HERE, aPath);
    } else if (fs::is_directory(cfgFile)) {
        throw FileIsDirectory(ERS_HERE, aPath);
    }

    //    return true;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
uint8_t
dec_rng(uint8_t word, uint8_t ibit, uint8_t nbits) {
    return (word >> ibit) & ((1<<nbits)-1);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
uint64_t
tstamp2int(uhal::ValVector< uint32_t > rawTimestamp) {
    return (uint64_t)rawTimestamp[0] + ((uint64_t)rawTimestamp[1] << 32);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
int64_t
get_seconds_since_epoch() {
    // get the current time
    const auto now = std::chrono::system_clock::now();

    // transform the time into a duration since the epoch
    const auto epoch = now.time_since_epoch();

    // cast the duration into seconds
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);

    // return the number of seconds
    return seconds.count();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
format_timestamp(uint64_t rawTimestamp) {
    std::time_t lSecFromEpoch = rawTimestamp / g_dune_sp_clock_in_hz;

    struct tm * lTime = localtime(&lSecFromEpoch);
    char lTimeBuffer[32];
    
    strftime(lTimeBuffer, sizeof lTimeBuffer, "%a, %d %b %Y %H:%M:%S +0000", lTime);
    return lTimeBuffer;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
std::string 
format_timestamp(uhal::ValVector< uint32_t > rawTimestamp) {
    uint64_t lTimestamp = tstamp2int(rawTimestamp);
    return format_timestamp(lTimestamp);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
double convert_bits_to_float(uint64_t aBits, bool aIsDoublePrecision){
    uint32_t lMantissaShift = aIsDoublePrecision ? 52 : 23;
    uint64_t lExponentMask = aIsDoublePrecision ? 0x7FF0000000000000 : 0x7f800000;
    uint32_t lBias = aIsDoublePrecision ? 1023 : 127;
    uint32_t lSignShift = aIsDoublePrecision ? 63 : 31;

    int32_t lSign = (aBits >> lSignShift) & 0x01;
    uint32_t lExponentBiased = ((aBits & lExponentMask) >> lMantissaShift);
    int32_t lExponent = lExponentBiased - lBias;

    int32_t lPower = -1;
    double lMantissa = 0.0;
    for (uint32_t i = 0; i < lMantissaShift; ++i) {
        uint64_t lMantissaBit = (aBits >> (lMantissaShift-i-1)) & 0x01;
        lMantissa += lMantissaBit * pow(2.0, lPower);
        --lPower;
    }
    
    if (lExponentBiased == 0) {
        ++lExponent;
        if (lMantissa == 0) return 0;
    } else {
        lMantissa += 1.0;    
    }
    return (lSign ? -1 : 1) * pow(2.0, lExponent) * lMantissa;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
BoardType
convert_value_to_board_type(uint32_t aBoardType) {
    // not pleasnt, but works for now
    if (aBoardType > kBoardTLU) {
        throw UnknownBoardType(ERS_HERE, format_reg_value(aBoardType));
    } else {
        return static_cast<BoardType> (aBoardType);
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
CarrierType
convert_value_to_carrier_type(uint32_t aCarrierType) {
    // not pleasnt, but works for now
    if (aCarrierType > kCarrierATFC) {
        throw UnknownCarrierType(ERS_HERE, format_reg_value(aCarrierType));
    } else {
        return static_cast<CarrierType> (aCarrierType);
    }
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
DesignType
convert_value_to_design_type(uint32_t aDesignType) {
    // not pleasnt, but works for now
    if (aDesignType > kDesingEndpoBICRT) {
        throw UnknownDesignType(ERS_HERE, format_reg_value(aDesignType));
    } else {
        return static_cast<DesignType> (aDesignType);
    }
}
//-----------------------------------------------------------------------------

template std::string pdt::vec_fmt<uint32_t>(const std::vector<uint32_t>& aVec);
template std::string pdt::short_vec_fmt<uint32_t>(const std::vector<uint32_t>& aVec);

//-----------------------------------------------------------------------------
uint32_t locate(float xx[], unsigned long n, float x) {
    uint32_t j, ju, jm, jl;
    int ascnd;
    jl = 0; //Initialize lower
    ju = n + 1; //and upper limits.
    ascnd = (xx[n] >= xx[1]);

    while (ju - jl > 1) //If we are not yet done,
    {
        jm = (ju + jl) >> 1; //compute a midpoint,

        if ((x >= xx[jm]) == ascnd) // added additional parenthesis
        {
            jl = jm; //and replace either the lower limit
        } else {
            ju = jm; //or the upper limit, as appropriate.
        }
    } //Repeat until the test condition is satisﬁed.

    if (x == xx[1]) {
        j = 1; //Then set the output
    } else if (x == xx[n]) {
        j = n - 1;
    } else {
        j = jl;
    }

    return j;
}
//-----------------------------------------------------------------------------

} // namespace pdt
} // namespace dunedaq