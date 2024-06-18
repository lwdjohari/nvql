#pragma once

#include <any>
#include <chrono>
#include <ostream>
#include <tuple>
#include <utility>
#include <vector>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/execution_result.h"
#include "nvserv/storages/row_result_iterator.h"

NVSERV_BEGIN_NAMESPACE(storages)

// // Helper function to call a function with a tuple of arguments
// template <typename Func, typename Tuple, std::size_t... I>
// decltype(auto) FnCallTuplesArgs(Func&& func, Tuple&& t,
//                                 std::index_sequence<I...>) {
//   return func(std::get<I>(std::forward<Tuple>(t))...);
// }

// template <typename Func, typename Tuple>
// decltype(auto) FnCallTuplesArgs(Func&& func, Tuple&& t) {
//   constexpr auto size = std::tuple_size<std::decay_t<Tuple>>::value;
//   return FnCallTuplesArgs(std::forward<Func>(func), std::forward<Tuple>(t),
//                           std::make_index_sequence<size>{});
// }

class Transaction {
 public:
  Transaction(StorageType type, TransactionMode mode){};
  virtual ~Transaction(){};

  /// @brief Execute SQL statement using NvQL. Behind-scenes NvQL data layer
  /// will execute using prepared statements and parameters. NvQL will manages
  /// all the wirings for using prepared statements and the binary transport
  /// protocol if supported by storage server.
  /// @tparam ...Args
  /// @param query
  /// @param ...args
  /// @return
  template <typename... Args>
  [[nodiscard]] ExecutionResultPtr Execute(const __NR_STRING_COMPAT_REF query,
                                           const Args&... args) {
    std::vector<std::any> anyArgs = {args...};
    return ExecuteImpl(query, anyArgs);
  }

  template <typename TParameterType>
  [[nodiscard]] ExecutionResultPtr Execute(
      const __NR_STRING_COMPAT_REF query,
      const std::vector<TParameterType>& args) {
    
    return ExecuteImpl(query, &args);
  }

  const StorageType& Type() const{
    return type_;
  }

  const TransactionMode& Mode() const{
    return mode_;
  }
  
  virtual void Commit() = 0;
  virtual void Rollback() = 0;


 protected:
  StorageType type_;
  TransactionMode mode_;
  // Non-template virtual function
  virtual ExecutionResultPtr ExecuteImpl(const __NR_STRING_COMPAT_REF query,
                                         const std::vector<std::any>& args) = 0;

  virtual ExecutionResultPtr ExecuteImpl(const __NR_STRING_COMPAT_REF query, void* args) = 0;
};

NVSERV_END_NAMESPACE