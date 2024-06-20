#pragma once

#include <exception>
#include <memory>
#include <pqxx/pqxx>

#include "nvm/random.h"
#include "nvserv/global_macro.h"
#include "nvserv/storages/connection.h"
#include "nvserv/storages/exceptions.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgConnection : public Connection {
 public:
  explicit PgConnection(
      const std::string& name, const ClusterConfigList& clusters,
      ConnectionStandbyMode standby_mode,
      std::chrono::seconds mark_idle_after = std::chrono::seconds(300))
                  : Connection(name, StorageType::Postgres, standby_mode,
                               mark_idle_after),
                    clusters_(clusters),
                    connection_string_(BuildConnectionString()),
                    conn_(nullptr),
                    mode_(),
                    hash_key_(CreateHashKey()) {}
  ~PgConnection() {}

  TransactionPtr CreateTransaction() override {
    return nullptr;
  }

  void Open() override {
    OpenImpl();
  }

  void Close() override {
    CloseImpl();
  }

  bool IsOpen() const override {
    if (!conn_)
      return false;

    return conn_->is_open();
  }

  ///< Ping server to report keep-alive.
  /// Different storage server has different implementations
  /// to guarantee the connection is keep-alive
  void PingServerAsync() override {
    std::cout << "Ping Server: " << GetHash() << std::endl;
  }

  virtual bool PingServer() override {
    std::cout << "Ping Server" << std::endl;
    return true;
  }

  TransactionMode SupportedTransactionMode() const override {
    return TransactionMode::ReadCommitted | TransactionMode::ReadOnly |
           TransactionMode::ReadWrite;
  }

  void ReportHealth() const override {
    std::cout << "Health Report" << std::endl;
  }

  ConnectionMode Mode() const override {
    return mode_;
  }

  void Release() override {
    ReleaseImpl();
  }

  size_t GetHash() const override {
    return hash_key_;
  }

  const std::string& GetConnectionString() const override {
    return connection_string_;
  };

  std::optional<std::pair<std::string, bool>> PrepareStatement(
      __NR_STRING_COMPAT_REF query) override {
    if (!prepared_statement_manager_)
      throw NullReferenceException(
          "Null reference to PrepareStatemenManagerPtr "
          "in prepared_statement_manager_");

    return prepared_statement_manager_->Register(query);
  }

  pqxx::connection* Driver() {
    return conn_.get();
  }

 protected:
  void OpenImpl() override {
    if (conn_)
      throw ConnectionException("Connection already created",
                                StorageType::Postgres);
    try {
      // std::cout << "Connection string:" << connection_string_ << std::endl;

      conn_ = std::make_unique<pqxx::connection>(connection_string_);

    } catch (const pqxx::broken_connection& e) {
      throw ConnectionException(e.what(), StorageType::Postgres);
    } catch (const pqxx::in_doubt_error& e) {
      throw InternalErrorException(e.what(), StorageType::Postgres);
    } catch (const pqxx::usage_error& e) {
      throw InternalErrorException(e.what(), StorageType::Postgres);
    } catch (const pqxx::internal_error& e) {
      throw InternalErrorException(e.what(), StorageType::Postgres);
    } catch (const std::exception& e) {
      throw ConnectionException(e.what(), StorageType::Postgres);
    } catch (...) {
      throw StorageException("Unknown exception caught.",
                             StorageType::Postgres);
    }
  }

  void CloseImpl() override {
    if (!conn_)
      return;
    if (!conn_->is_open())
      return;

    try {
      // Explicitly close the connection
      conn_->close();
      std::cout << "Connection to database closed successfully." << std::endl;
    } catch (const pqxx::broken_connection& e) {
      throw ConnectionException(e.what(), StorageType::Postgres);
    } catch (const std::exception& e) {
      throw ConnectionException(e.what(), StorageType::Postgres);
    } catch (...) {
      throw StorageException(
          "Unknown exception caught during connection close.",
          StorageType::Postgres);
    }
  }

  void ReleaseImpl() override {
    if (!conn_)
      return;

    Close();
    conn_ = nullptr;
  }

 private:
  ClusterConfigList clusters_;
  std::string connection_string_;
  std::unique_ptr<pqxx::connection> conn_;
  ConnectionMode mode_;
  std::hash<std::string> hash_fn_;
  size_t hash_key_;

  size_t CreateHashKey() {
    std::ostringstream ss;
    ss << "{" << ToStringEnumStorageType(type_) << " | "
       << std::to_string(nvm::utils::RandomizeUint32t()) << "}";
       
    auto feed = ss.str();
    auto hash_key = hash_fn_(feed);
    
    return hash_key;
  }

  std::string BuildConnectionString() {
    if (clusters_.Configs().size() == 0)
      return std::string();

    std::ostringstream host;
    std::string dbname;

    uint16_t index = 0;

    for (auto cluster : clusters_.Configs()) {
      auto pg_cluster = std::dynamic_pointer_cast<PgClusterConfig>(cluster);
      if (index == 0) {
        host << "postgresql://" << std::string(pg_cluster->User()) << ":"
             << std::string(pg_cluster->Password()) << "@";
        dbname = std::string(pg_cluster->DbName());

        host << pg_cluster->Host() << ":" << pg_cluster->Port();
        index++;
        continue;
      }

      host << "," << pg_cluster->Host() << ":" << pg_cluster->Port();

      index++;
    }

    host << "/" << dbname << "?application_name=" << name_ << "::nvql"
         << "&connect_timeout=5";

    return __NR_RETURN_MOVE(host.str());
  }
};

NVSERV_END_NAMESPACE