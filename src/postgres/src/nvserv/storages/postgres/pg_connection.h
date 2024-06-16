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
      ConnectionStandbyMode standby_mode,
      std::chrono::seconds mark_idle_after = std::chrono::seconds(300))
                  : Connection(StorageType::Postgres, standby_mode,
                               mark_idle_after),
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
    throw std::runtime_error("Unimplemented");
  }

  virtual bool PingServer() override {
    throw std::runtime_error("Unimplemented");
  }

  TransactionMode SupportedTransactionMode() const override {
    return TransactionMode::ReadCommitted | TransactionMode::ReadOnly |
           TransactionMode::ReadWrite;
  }

  void ReportHealth() const override {
    throw std::runtime_error("Unimplemented");
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

  size_t PrepareStatement(__NR_STRING_COMPAT_REF query) override {
    if (!prepared_statement_manager_)
      throw NullReferenceException(
          "Null reference to PrepareStatemenManagerPtr "
          "in prepared_statement_manager_");
#if __NR_CPP17
    auto query_ref = reinterpret_cast<const std::string*>(query.data());
    auto key = hash_fn_(*query_ref);
#else
    auto key = hash_fn_(query);
#endif

    auto exist = prepared_statement_manager_->IsExist(key);
    if (exist)
      return key;

#if __NR_CPP17
    return prepared_statement_manager_->Register(*query_ref).value();
#else
    return prepared_statement_manager_->Register(query);
#endif
  }

 protected:
  void OpenImpl() override {
    if (conn_)
      throw ConnectionException("Connection already created",
                                          StorageType::Postgres);
    try {
      conn_ = std::make_unique<pqxx::connection>(
          "dbname=mydb user=myuser password=mypass hostaddr=127.0.0.1 "
          "port=5432");
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
  std::unique_ptr<pqxx::connection> conn_;
  ConnectionMode mode_;
  std::hash<std::string> hash_fn_;
  size_t hash_key_;

  size_t CreateHashKey() {
    std::ostringstream ss;
    ss << ToStringEnumStorageType(type_) << " | "
       << std::to_string(nvm::utils::RandomizeUint32t()) << " | "
       << std::to_string(nvm::utils::RandomizeUint32t());
    auto hash_str = ss.str();
    std::cout << "Connection Hash Key:" << hash_str << std::endl;
    return hash_fn_(hash_str);
  }
};

NVSERV_END_NAMESPACE