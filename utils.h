#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <cstdint>
#include <format>
#include <string>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

enum sort_criteria : u8
{
  TITLE = 0,
  DURATION
};

inline std::string format_duration(const float &time)
{
  int minutes = static_cast<int>(time);
  int seconds = std::round((time - minutes) * 100.0f);
  if (seconds >= 60)
  {
    minutes += seconds / 60;
    seconds %= 60;
  }
  if (minutes == 0)
    return std::format("{}s", seconds);
  else if (seconds == 0)
    return std::format("{}m", minutes);
  else
    return std::format("{}m{}s", minutes, seconds);
}

#endif