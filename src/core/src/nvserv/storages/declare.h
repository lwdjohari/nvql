
#pragma once

#include <nvm/macro.h>

#include <ostream>
#include <vector>

#include "nvserv/global_macro.h"

NVSERV_BEGIN_NAMESPACE(storages)

class StorageServer;
class Transaction;
class ClusterConfig;
class ExecutionResult;
class RowResult;
class PreparedStatementManager;
class StorageConfig;
class Connection;
class ConnectionPool;

namespace postgres {
class PgServer;
}

using StorageServerPtr = std::shared_ptr<StorageServer>;
using TransactionPtr = std::shared_ptr<Transaction>;
using ConnectionPoolPtr = std::shared_ptr<ConnectionPool>;
using ClusterConfigListType = std::vector<std::shared_ptr<ClusterConfig>>;
using ExecutionResultPtr = std::shared_ptr<ExecutionResult>;
using RowResultPtr = std::shared_ptr<RowResult>;
using PreparedStatementManagerPtr = std::shared_ptr<PreparedStatementManager>;
using ConnectionPtr = std::shared_ptr<Connection>;

/// @brief NvServ storage driver
enum class StorageType {
  Unknown = 0,   // Storage driver is uninitialized
  Postgres = 1,  // Postgres Database Server
  Oracle = 2,    // Oracle Database Server
  Mysql = 3,     // Mysql is not yet supported
  SqlLite = 4,   // Sqlite is not yet supported
  Redis = 5,     // Redis is not yet supported
  Parquet = 6,   // Parquet is not yet supported
  NvXcel = 7     // NvXcel is not yet supported
};

NVM_ENUM_CLASS_DISPLAY_TRAIT(StorageType)

NVM_ENUM_TO_STRING_FORMATTER(StorageType, case StorageType::Unknown
                             // cppcheck-suppress syntaxError
                             : return "Unknown";
                             case StorageType::Postgres
                             : return "Postgres";
                             case StorageType::Oracle
                             : return "Oracle";
                             case StorageType::Mysql
                             : return "Mysql";
                             case StorageType::SqlLite
                             : return "SqlLite";
                             case StorageType::Redis
                             : return "Redis";
                             case StorageType::Parquet
                             : return "Parquet";
                             case StorageType::NvXcel
                             : return "NvXcel";)

/// @brief TransactionMode to set,
/// each features in certain DB if the characteristics of transaction is not far
/// different
// from other concept one that fits inot that category will be created the
// fallback mechanism.
/// For one that only specific to that DB implementations might throw an
/// exceptions.
enum class TransactionMode : uint16_t {
  // No TransactionMode supported
  Unknown = 0,
  // R/W operation
  //
  // Support: pg, oracle, mysql
  ReadWrite = 1,
  // Read only commited data
  //
  // Support: pg, oracle, mysql
  ReadCommitted = 2,
  // Read only operations in pg, fallback to ReadCommitted in oracle & mysql
  //
  // Support: pg.
  // Fallback: ReadCommitted in oracle & mysql.
  ReadOnly = 4
};

NVM_ENUMCLASS_ENABLE_BITMASK_OPERATORS(TransactionMode)

NVM_ENUM_CLASS_DISPLAY_TRAIT(TransactionMode)

NVM_ENUM_TO_STRING_FORMATTER(TransactionMode, case TransactionMode::Unknown
                             : return "Unknown";
                             case TransactionMode::ReadWrite
                             : return "ReadWrite";
                             case TransactionMode::ReadCommitted
                             : return "ReadCommitted";
                             case TransactionMode::ReadOnly
                             : return "ReadOnly";)

enum class ConnectionMode {
  Unknown = 0,
  Server = 1,
  ServerCluster = 2,
  File = 4
};

NVM_ENUM_CLASS_DISPLAY_TRAIT(ConnectionMode);

NVM_ENUM_TO_STRING_FORMATTER(ConnectionMode, case ConnectionMode::Unknown
                             : return "Unknown";
                             case ConnectionMode::Server
                             : return "Server";
                             case ConnectionMode::ServerCluster
                             : return "ServerCluster";
                             case ConnectionMode::File
                             : return "File";)

enum class ConnectionStandbyMode { None = 0, Primary = 1, Standby = 2 };

NVM_ENUM_CLASS_DISPLAY_TRAIT(ConnectionStandbyMode)

NVM_ENUM_TO_STRING_FORMATTER(ConnectionStandbyMode,
                             case ConnectionStandbyMode::None
                             : return "None";
                             case ConnectionStandbyMode::Primary
                             : return "Primary";
                             case ConnectionStandbyMode::Standby
                             : return "Standby";)


class StorageInfo {
 public:
  StorageInfo()
                  : name_(),
                    version_(),
                    info_(),
                    type_(),
                    is_trans_supported_() {}

  ~StorageInfo();

  virtual const __NR_STRING_COMPAT_REF Name() const {
    return name_;
  };

  virtual const __NR_STRING_COMPAT_REF Version() const {
    return version_;
  };

  virtual const __NR_STRING_COMPAT_REF Info() const {
    return info_;
  };

  virtual bool TransactionSupported() const {
    return is_trans_supported_;
  };

  virtual StorageType Type() const {
    return type_;
  };

  virtual std::string GetServerInfo() {
    return "Unimplemented";
  };

 protected:
  std::string name_;
  std::string version_;
  std::string info_;
  StorageType type_;
  bool is_trans_supported_;
};

NVSERV_END_NAMESPACE