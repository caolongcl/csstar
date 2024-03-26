#pragma once

// std
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <span>
#include <optional>
#include <sstream>
#include <ranges>
#include <cstring>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <exception>
#include <stdexcept>
#include <utility>
#include <fstream>
#include <stack>
#include <queue>
#include <utility>
#include <chrono>

namespace dsg
{
  inline std::string_view ParseFileNameFromPath(std::string_view path)
  {
    const auto start = path.find_last_of('/');
    return {path.data() + start + 1, path.length() - start - 1};
  }

  inline std::string_view ParseFileNameFromPathWithoutExt(std::string_view path)
  {
    const auto start = path.find_last_of('/');
    const auto end = path.find_last_of('.');
    return {path.data() + start + 1, end - start - 1};
  }

  inline bool StrEqual(const char *l, const char *r)
  {
    return std::string_view(l) == std::string_view(r);
  }

  inline std::string CurTime()
  {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    return std::ctime(&now);
  }

  inline int64_t CurTimestamp()
  {
    return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
  }

#define DSG_STR(_in_stream) ((std::stringstream() << _in_stream).str())

#define LOG_INFO " [" << dsg::ParseFileNameFromPathWithoutExt(__FILE__) << ", " << __func__ << ":" << __LINE__ << "]"

#define DSG_ERROR(msg) std::cerr << DSG_STR("[DSG E " << CurTimestamp() << "] " << msg << LOG_INFO << "\n");
#define DSG_LOG(msg) std::clog << DSG_STR("[DSG D " << CurTimestamp() << "] " << msg << "\n");
#define DSG_WARN(msg) std::clog << DSG_STR("[DSG W " << CurTimestamp() << "] " << msg << LOG_INFO << "\n");
#define DSG_THROW(msg) throw std::runtime_error(DSG_STR(msg << "\n"));

#define DSG_TRACE(msg)
  // #define DSG_TRACE(msg) std::clog << DSG_STR("[DSG T " << CurTimestamp() << "] " << msg << LOG_INFO << "\n");

#define DSG_CALL_EX(funccall) \
  {                           \
    if (!(funccall))          \
    {                         \
      DSG_THROW(#funccall)    \
    }                         \
  }

#define DSG_CALL(funccall) \
  {                        \
    if (!(funccall))       \
    {                      \
      DSG_ERROR(#funccall) \
      return DSG_Err;      \
    }                      \
  }

#define DSG_CALLM(funccall, msg)         \
  {                                      \
    if (!(funccall))                     \
    {                                    \
      DSG_ERROR(#funccall << " " << msg) \
      return DSG_Err;                    \
    }                                    \
  }
}

//////////////////////////////////////////////////////////////////
