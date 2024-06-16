#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <pqxx/pqxx>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

#include "nvserv/global_macro.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgPool final {
 public:
  PgPool(const std::vector<std::string>& cluster_conninfo,
         const std::string& fallback_conninfo, int min_connections,
         int max_connections)
                  : cluster_conninfo_(cluster_conninfo),
                    fallback_conninfo_(fallback_conninfo),
                    min_connections_(min_connections),
                    max_connections_(max_connections) {
    for (int i = 0; i < min_connections_; ++i) {
      connections_.emplace(CreateConnection());
    }
  }
};

NVSERV_END_NAMESPACE