#pragma once

#include <any>
#include <chrono>
#include <ostream>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/execution_result.h"
#include "nvserv/storages/row_result_iterator.h"

NVSERV_BEGIN_NAMESPACE(storages)

namespace parameters {

using NvQLVType = std::variant<
    std::reference_wrapper<const int16_t>,      // Small Int
    std::reference_wrapper<const int32_t>,      // Integer type
    std::reference_wrapper<const int64_t>,      // Bigint
    std::reference_wrapper<const double>,       // Floating-point type
    std::reference_wrapper<const float>,        // Real (float) type
    std::reference_wrapper<const std::string>,  // String type, including JSON
                                                // and JSONB
    std::reference_wrapper<const bool>,         // Boolean type
    std::reference_wrapper<
        const std::chrono::system_clock::time_point>,  // Timestamp type
    std::reference_wrapper<const NvDateTime>,          // Timestamp offset
    std::reference_wrapper<const std::vector<unsigned char>>,    // Binary data
                                                                 // type
    std::reference_wrapper<const std::array<unsigned char, 16>>  // UUID type
    >;

enum class DataType {
  SmallInt,
  Int,
  BigInt,
  Real,
  Double,
  String,
  Boolean,
  Date,
  Time,
  Timestamp,
  Timestampz
};

class Param {
 public:
  explicit Param(const int16_t& value)
                  : data_(std::cref(value)), type_(DataType::SmallInt) {}
  explicit Param(const int32_t& value)
                  : data_(std::cref(value)), type_(DataType::Int) {}
  explicit Param(const int64_t& value)
                  : data_(std::cref(value)), type_(DataType::BigInt) {}
  explicit Param(const double& value)
                  : data_(std::cref(value)), type_(DataType::Double) {}
  explicit Param(const float& value)
                  : data_(std::cref(value)), type_(DataType::Real) {}
  explicit Param(const std::string& value)
                  : data_(std::cref(value)), type_(DataType::String) {}
  explicit Param(const bool& value)
                  : data_(std::cref(value)), type_(DataType::Boolean) {}

  explicit Param(const std::chrono::system_clock::time_point& value)
                  : data_(std::cref(value)), type_(DataType::Timestamp) {}
  explicit Param(const NvDateTime& value)
                  : data_(std::cref(value)), type_(DataType::Timestampz) {}

  static Param SmallInt(const int16_t& value) {
    return Param(value);
  }
  static Param Int(const int32_t& value) {
    return Param(value);
  }
  static Param BigInt(const int64_t& value) {
    return Param(value);
  }
  static Param Double(const double& value) {
    return Param(value);
  }
  static Param Real(const float& value) {
    return Param(value);
  }
  static Param String(const std::string& value) {
    return Param(value);
  }
  static Param Bool(const bool& value) {
    return Param(value);
  }

  static Param Timestamp(const std::chrono::system_clock::time_point& value) {
    return Param(value);
  }
  static Param Timestampz(const NvDateTime& value) {
    return Param(value);
  }

  // explicit Param(const char* value)
  //                 : data_(std::cref(std::string(value))),
  //                   type_(DataType::String) {}

  // explicit Param(const std::vector<unsigned char>& value)
  //                 : data_(std::cref(value)), type_(DataType::Date) {}
  // explicit Param(const std::array<unsigned char, 16>& value)
  //                 : data_(std::cref(value)), type_(DataType::Date) {}

  DataType Type() const {
    return type_;
  }

  template <typename T>
  const T& As() const {
    return std::get<std::reference_wrapper<const T>>(data_).get();
  }

 private:
  NvQLVType data_;
  DataType type_;
};

using ParameterArgs = std::vector<Param>;

}  // namespace parameters

class Transaction {
 public:
  Transaction(StorageType type, TransactionMode mode):type_(type),mode_(mode) {};
  virtual ~Transaction(){};

  /// @brief Execute SQL statement using NvQL. Behind-scenes NvQL data layer
  /// will execute using prepared statements and parameters. NvQL will manages
  /// all the wirings for using prepared statements and the binary transport
  /// protocol if supported by storage server.
  /// @tparam ...Args
  /// @param query
  /// @param ...args
  /// @return
  // template <typename... Args>
  // [[nodiscard]] ExecutionResultPtr Execute(const __NR_STRING_COMPAT_REF query,
  //                                          const Args&... args) {
  //   std::vector<std::any> anyArgs = {args...};
  //   return ExecuteImpl(query, anyArgs);
  // }

  [[nodiscard]] ExecutionResultPtr Execute(
      const __NR_STRING_COMPAT_REF query) {
    return ExecuteImpl(query, parameters::ParameterArgs());
  }

  [[nodiscard]] ExecutionResultPtr Execute(
      const __NR_STRING_COMPAT_REF query,
      const parameters::ParameterArgs& args) {
    return ExecuteImpl(query, args);
  }

  const StorageType& Type() const {
    return type_;
  }

  const TransactionMode& Mode() const {
    return mode_;
  }

  virtual void Commit() = 0;
  virtual void Rollback() = 0;

 protected:
  StorageType type_;
  TransactionMode mode_;
  // Non-template virtual function
  // virtual ExecutionResultPtr ExecuteImpl(const __NR_STRING_COMPAT_REF query,
  //                                        const std::vector<std::any>& args) = 0;

  virtual ExecutionResultPtr ExecuteImpl(const __NR_STRING_COMPAT_REF query,
                                        const parameters::ParameterArgs& args) = 0;
};

NVSERV_END_NAMESPACE