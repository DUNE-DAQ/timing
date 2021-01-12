/**
 * @file    Logger.hpp
 * @author  Alessandro Thea
 * @date    November 2014
 */

#ifndef __PDT_LOGGER_LOG_HPP__
#define __PDT_LOGGER_LOG_HPP__

#include <sstream>
#include <map>
#include <vector>
#include <stdint.h>

// Macro Declaration
#define PDT_LOG(level) \
if (level > pdt::Log::logThreshold()) ; \
else pdt::Log().get(level)

// Forward declarations
namespace uhal{
  template <class T> class ValWord;
}


namespace pdt {

// Log, version 0.1: a simple logging class

enum LogLevel {
    kError,
    kWarning,
    kNotice,
    kInfo,
    kDebug,
    kDebug1,
    kDebug2,
    kDebug3,
    kDebug4
};

class Log {
public:
    Log();
    virtual ~Log();
    std::ostringstream& get(LogLevel level = kInfo);
public:
    static LogLevel& logThreshold();
    static void setLogThreshold( LogLevel level ); 
    // static const std::string& toString(LogLevel level);

protected:
    void push( LogLevel level, const std::string& source, const std::string& message );

    std::ostringstream os_;

private:
    Log(const Log&);
    Log& operator=(const Log&);

private:
    LogLevel messageLevel_;

    static LogLevel logThreshold_;

    // static const std::map<LogLevel, std::string> logNames_;
    static const char* logNames_[];
};


std::string boolFmt(const bool );

std::string hexFmt(const uint32_t );

std::string hexFmt(const uhal::ValWord<uint32_t>& );

template<typename T>
extern std::string vecFmt(const std::vector<T>& aVec);

template<typename T>
extern std::string shortVecFmt(const std::vector<T>& aVec);

} // pdt

#endif /* __PDT_LOGGER_LOG_HPP__ */