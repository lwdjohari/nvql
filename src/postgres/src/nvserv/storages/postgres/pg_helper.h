#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "nvserv/global_macro.h"

//cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages::postgres::helper)

std::chrono::system_clock::time_point ParseTimestamp(
    const std::string& timestamp) {
  std::istringstream ss(timestamp);
  std::tm tm = {};
  ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
  std::time_t time_c = std::mktime(&tm);
  return std::chrono::system_clock::from_time_t(time_c);
}

std::chrono::system_clock::time_point ParseTimestampz(
    const std::string& timestamp) {
  std::istringstream ss(timestamp);
  std::tm tm = {};
  ss >> std::get_time(&tm,
                      "%Y-%m-%d %H:%M:%S");  // Adjust the format if necessary

  // Handle timezone offset if present
  std::string tz_offset;
  ss >> tz_offset;
  if (!tz_offset.empty()) {
    int hours = std::stoi(tz_offset.substr(0, 3));
    int minutes = std::stoi(tz_offset.substr(4, 2));
    tm.tm_hour -= hours;
    tm.tm_min -= minutes;
  }

  std::time_t time_c = std::mktime(&tm);
  return std::chrono::system_clock::from_time_t(time_c);
}

NVSERV_END_NAMESPACE