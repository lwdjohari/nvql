#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <type_traits>
#include <utility>
#include <memory>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/storages/row_result.h"
NVSERV_BEGIN_NAMESPACE(storages)
namespace Mapper {

// Forward declaration of GetValue function template
template <typename T>
T GetValue(const std::shared_ptr<RowResult> row, const std::string& column_name);

// Helper to get index sequence
template <typename Tuple, std::size_t... Is>
inline auto MakeTupleFromRow(const std::shared_ptr<RowResult> row,
                      const std::vector<std::string>& column_names,
                      std::index_sequence<Is...>) {
  return std::make_tuple(
      (GetValue<typename std::tuple_element<Is, Tuple>::type>(
          row, column_names[Is]))...);
}

// Function to dynamically create a tuple from a row
template <typename... Args>
inline auto Dynamic(const std::shared_ptr<RowResult> row,
             const std::vector<std::string>& column_names) {
  return MakeTupleFromRow<std::tuple<Args...>>(
      row, column_names, std::index_sequence_for<Args...>{});
}

// Helper function to get the value from the row
template <typename T>
inline T GetValue(const std::shared_ptr<RowResult> row,
           const std::string& column_name) {
  return row->As<T>(column_name);
}


// // Helper to map tuple to model
// template <typename Model, typename Tuple, std::size_t... Is>
// Model MapImpl(const Tuple& tuple, std::index_sequence<Is...>) {
//   return Model{std::get<Is>(tuple)...};
// }

// // Function to map tuple to model
// template <typename Model, typename Tuple>
// Model Map(const Tuple& tuple) {
//   constexpr auto size = std::tuple_size<Tuple>::value;
//   return MapImpl<Model>(tuple, std::make_index_sequence<size>{});
// }



}  // namespace Mapper

NVSERV_END_NAMESPACE