/*
 * Copyright (c) 2024 Linggawasistha Djohari
 * <linggawasistha.djohari@outlook.com>
 * Licensed to Linggawasistha Djohari under one or more contributor license
 * agreements.
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership.
 *
 *  Linggawasistha Djohari licenses this file to you under the Apache License,
 *  Version 2.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "nvserv/storages/postgres/pg_helper.h"

#include <ctime>
#include <iomanip>
#include <sstream>

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
  ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

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