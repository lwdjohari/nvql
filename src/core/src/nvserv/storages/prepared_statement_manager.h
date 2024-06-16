#pragma once

#include <absl/container/node_hash_map.h>
#include <nvm/dates/datetime.h>
#include <nvm/macro.h>
#include <nvm/strings/utility.h>

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "nvserv/global_macro.h"
#include "nvserv/headers/absl_thread.h"
#include "nvserv/storages/declare.h"
NVSERV_BEGIN_NAMESPACE(storages)

class PreparedStatementItem {
 public:
  explicit PreparedStatementItem(
      size_t statement_key, const std::string& name, const std::string& query,
      const std::chrono::system_clock::time_point& created)
                  : statement_key_(statement_key),
                    name_(name),
                    query_(std::string(query)),
                    created_time_(
                        std::chrono::system_clock::time_point(created)) {}
  __NR_STRING_COMPAT_REF Query() const {
    return query_;
  }

  const std::string& Name() const {
    return name_;
  }

  const size_t& Key() const {
    return statement_key_;
  }

  std::chrono::system_clock::time_point CreatedTime() const {
    return created_time_;
  }

 private:
  size_t statement_key_;
  std::string name_;
  std::string query_;
  std::chrono::system_clock::time_point created_time_;
};

/// @brief Managing prepared statement query, one connection will be have one
/// PreparedStatementManager.
class PreparedStatementManager : public std::enable_shared_from_this<PreparedStatementManager> {
 public:
  PreparedStatementManager(){};
  ~PreparedStatementManager(){};

  std::optional<size_t> Register(
                                 const std::string& query) {
    if (query.empty())
      return std::nullopt;

    if (nvm::strings::utility::IsWhitespaceString(query))
      return std::nullopt;

    auto key = hash_fn_(query);

    if (IsExist(key))
      return false;

    statements_.emplace(
        key, std::move(PreparedStatementItem(
                 key, std::string(std::to_string(key)), std::string(query),
                 nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time())));
    return key;
  }

  std::optional<size_t> Register(const std::string& name,
                                 const std::string& query) {
    if (query.empty())
      return std::nullopt;

    if (nvm::strings::utility::IsWhitespaceString(query))
      return std::nullopt;

    auto key = hash_fn_(query);

    if (IsExist(key))
      return false;

    statements_.emplace(
        key, std::move(PreparedStatementItem(
                 key, std::string(name), std::string(query),
                 nvm::dates::DateTime::UtcNow().TzTime()->get_sys_time())));
    return key;
  }

  

  bool IsExist(const size_t& statement_key) const {
    return statements_.contains(statement_key);
  }

  bool IsExist(const std::string& query) const {
    auto key = hash_fn_(query);
    return IsExist(key);
  }

  PreparedStatementManagerPtr Share() {
    return this->shared_from_this();
  }

 private:
  absl::node_hash_map<size_t, PreparedStatementItem> statements_;
  std::hash<std::string> hash_fn_;
  absl::Mutex mutex_;
};

NVSERV_END_NAMESPACE
