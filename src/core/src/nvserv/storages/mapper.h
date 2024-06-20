#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/row_result.h"
NVSERV_BEGIN_NAMESPACE(storages)
namespace Mapper {

namespace impl {
// Forward declaration of GetValue function template
template <typename T>
T GetValue(const std::shared_ptr<RowResult> row,
           const std::string& column_name);

// Helper to get index sequence
template <typename Tuple, std::size_t... Is>
inline auto MakeTupleFromRow(const std::shared_ptr<RowResult> row,
                             const std::vector<std::string>& column_names,
                             std::index_sequence<Is...>) {
  return std::make_tuple(
      (GetValue<typename std::tuple_element<Is, Tuple>::type>(
          row, column_names[Is]))...);
}

// Helper function to get the value from the row
template <typename T>
inline T GetValue(const std::shared_ptr<RowResult> row,
                  const std::string& column_name) {
  return row->As<T>(column_name);
}

// Helper to map tuple to model
template <typename TModel, typename TTuple, std::size_t... Is>
inline TModel MapImpl(const TTuple& tuple, std::index_sequence<Is...>) {
  return __NR_RETURN_MOVE(TModel{std::get<Is>(tuple)...});
}

}  // namespace impl

// Dynamically create a tuple from a row & selected column
template <typename... Args>
inline auto Dynamic(const std::shared_ptr<RowResult> row,
                    const std::vector<std::string>& column_names) {
  return impl::MakeTupleFromRow<std::tuple<Args...>>(
      row, column_names, std::index_sequence_for<Args...>{});
}

// Function to map tuple to model
template <typename TModel, typename TTuple>
inline TModel Map(const TTuple& tuple) {
  constexpr auto size = std::tuple_size<TTuple>::value;
  return impl::MapImpl<TModel>(tuple, std::make_index_sequence<size>{});
}

}  // namespace Mapper

NVSERV_END_NAMESPACE