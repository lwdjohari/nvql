#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <pqxx/pqxx>
#include <stdexcept>
#include <vector>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/postgres/pg_column.h"
#include "nvserv/storages/row_result.h"
#include "nvserv/storages/postgres/pg_helper.h"
NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgRowResult : public RowResult {
 public:
  explicit PgRowResult(const pqxx::row& row) : RowResult(), row_(row) {}
  virtual ~PgRowResult() = default;
  std::optional<Column> GetColumn(
      const std::string& columnName) const override {
    try {
      return PgColumn(row_[columnName]);
    } catch (const std::exception& e) {
      return std::nullopt;
    }
  }

  std::optional<Column> GetColumn(const size_t& index) const override {
    if (index >= row_.size()) {
      return std::nullopt;
    }
    return PgColumn(row_.at(index));
  }

  size_t Size() const override{
    return row_.size();
  }

  // ColumnIterator begin() const {
  //     return ColumnIterator(row_, 0);
  // }

  // ColumnIterator end() const {
  //     return ColumnIterator(row_, row_.size());
  // }

  int16_t AsImpl_int16_t(const size_t& index) const override {
    try {
      return row_.at(index).as<int16_t>();
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  int32_t AsImpl_int32_t(const size_t& index) const override {
    try {
      return row_.at(index).as<int32_t>();
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  int64_t AsImpl_int64_t(const size_t& index) const override {
    try {
      return row_.at(index).as<int64_t>();
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  std::string AsImpl_string(const size_t& index) const override {
    try {
      return row_.at(index).as<std::string>();
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  float AsImpl_float(const size_t& index) const override {
    try {
      return row_.at(index).as<float>();
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  double AsImpl_double(const size_t& index) const override {
    try {
      return row_.at(index).as<double>();
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  nvm::dates::DateTime AsImpl_DateTime_Timestampz(
      const size_t& index) const override {
    try {
      auto time_point =
          helper::ParseTimestampz(row_.at(index).as<std::string>());
      return nvm::dates::DateTime(time_point, "Etc/Utc");
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  nvm::dates::DateTime AsImpl_DateTime_Timestamp(
      const size_t& index) const override {
    try {
      auto time_point =
          helper::ParseTimestamp(row_.at(index).as<std::string>());
      return nvm::dates::DateTime(time_point, "Etc/Utc");
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  int16_t AsImpl_int16_t(const std::string& column_name) const override {
    try {
      return row_.at(column_name).as<int16_t>();
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  int32_t AsImpl_int32_t(const std::string& column_name) const override {
    try {
      return row_.at(column_name).as<int32_t>();
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  int64_t AsImpl_int64_t(const std::string& column_name) const override {
    try {
      return row_.at(column_name).as<int64_t>();
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  std::string AsImpl_string(const std::string& column_name) const override {
    try {
      return __NR_RETURN_MOVE(row_.at(column_name).as<std::string>());
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  float AsImpl_float(const std::string& column_name) const override {
    try {
      return row_.at(column_name).as<float>();
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  double AsImpl_double(const std::string& column_name) const override {
    try {
      return row_.at(column_name).as<double>();
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  nvm::dates::DateTime AsImpl_DateTime_Timestampz(
      const std::string& column_name) const override {
    try {
      auto time_point =
          helper::ParseTimestampz(row_.at(column_name).as<std::string>());
      return __NR_RETURN_MOVE(nvm::dates::DateTime(time_point, "Etc/Utc"));
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

  nvm::dates::DateTime AsImpl_DateTime_Timestamp(
      const std::string& column_name) const override {
    try {
      auto time_point =
          helper::ParseTimestamp(row_.at(column_name).as<std::string>());
      return __NR_RETURN_MOVE(nvm::dates::DateTime(time_point, "Etc/Utc"));
    } catch (const std::exception& e) {
      // std::cerr << e.what() << '\n';
      throw std::invalid_argument(e.what());
    }
  };

 private:
  const pqxx::row& row_;
};

NVSERV_END_NAMESPACE
