#include <iostream>
#include <memory>

#include "nvm/stopwatch.h"
#include "nvserv/storages/postgres/pg_server.h"

void ParamsTest() {
  using namespace nvserv::storages;
  using namespace nvserv::storages::parameters;

  std::cout << "NvQL Params Test\n" << std::endl;

  ParameterArgs params = {Param::SmallInt(16), Param::String("Hey world"),
                          Param::Timestampz(nvserv::NvDateTime::UtcNow())};

  for (const auto& param : params) {
    switch (param.Type()) {
      case DataType::SmallInt:
        std::cout << "SmallInt: " << param.As<int16_t>() << std::endl;
        break;
      case DataType::Int:
        std::cout << "Int: " << param.As<int32_t>() << std::endl;
        break;
      case DataType::BigInt:
        std::cout << "BigInt: " << param.As<int64_t>() << std::endl;
        break;
      case DataType::Double:
        std::cout << "Double: " << param.As<double>() << std::endl;
        break;
      case DataType::Real:
        std::cout << "Real: " << param.As<float>() << std::endl;
        break;
      case DataType::String:
        std::cout << "String: " << param.As<std::string>() << std::endl;
        break;
      case DataType::Boolean:
        std::cout << "Boolean: " << param.As<bool>() << std::endl;
        break;
      case DataType::Timestamp:
        std::cout << "Timestamp: "
                  << param.As<std::chrono::system_clock::time_point>()
                         .time_since_epoch()
                         .count()
                  << std::endl;
        break;
      case DataType::Timestampz:
        std::cout << "Timestampz: " << param.As<nvserv::NvDateTime>()
                  << std::endl;
        break;
      case DataType::Date:
        std::cout << "Binary data: ";
        for (auto c : param.As<std::vector<unsigned char>>()) {
          std::cout << static_cast<int>(c) << " ";
        }
        std::cout << std::endl;
        break;
      default:
        std::cout << "Unknown type" << std::endl;
        break;
    }
  }

  auto utc = Param::Timestampz(nvserv::NvDateTime::UtcNow());
  std::cout << "Param Size: " << sizeof(utc) << "\n" << std::endl;
}

struct User {
  int32_t user_id;
  std::string username;
  int16_t status;
};

int main() {
  ParamsTest();

  using namespace nvserv::storages;
  using namespace nvserv::storages::parameters;

  std::cout << "NvQL Transaction Test\n" << std::endl;

  // Only clusters, server declarations and dialect that are DB Specific
  // For NvQL executions are abstracted through unified API.

  // Connect to standalone Postgress DB Server
  // Connection pool with 5 connections standby, max 10 connections
  auto clusters = {postgres::PgClusterConfig("minechain", "mnvx", "mnvx",
                                             "172.21.240.1", 5433)};

  StorageServerPtr server =
      postgres::PgServer::MakePgServer("nvql-pg", clusters, 1, 1);
  try {
    nvm::Stopwatch sw;

    server->TryConnect();
    std::cout << "Connect time: " << sw.ElapsedMilliseconds() << "ms\n"
              << std::endl;
    {
      auto transact_mode = TransactionMode::ReadOnly;
      std::cout << "Create DB Transaction: "
                << ToStringEnumTransactionMode(transact_mode) << std::endl;

      sw.Reset();
      auto tx = server->Begin(transact_mode);

      std::cout << "Transaction Creation: " << sw.ElapsedMilliseconds()
                << "ms\n"
                << std::endl;
      sw.Reset();

      std::string query = "select * from users u inner join company c \n"
                          "on u.company_id = c.company_id \n"
                          "where u.status = $1 and u.user_id = $2";
      std::cout << "Prepare SQL Query statement: \n"
                << query << "\n"
                << std::endl;

      auto status_filter = Param::SmallInt(1);
      auto user_id_filter = Param::Int(2);

      nvm::Stopwatch swt;
      for (size_t i = 0; i < 10; i++) {
        ExecutionResultPtr result =
            tx->ExecuteNonPrepared(query, {status_filter, user_id_filter});

        if (result->Empty()) {
          std::cout << "No customers data.." << std::endl;
          server->Shutdown();
          return 0;
        }

        auto cursor = Cursor(*result);
        for (const auto row : cursor) {
          auto dyn = Mapper::Dynamic<int32_t, std::string, int16_t>(
              row, {"user_id", "username", "status"});

          auto u = Mapper::Map<User, typeof(dyn)>(dyn);

          auto cust_id = u.user_id;  // row->As<int32_t>("user_id");
          auto name = u.username;    // row->As<std::string>("username");
          auto status = u.status;    // row->As<int16_t>("status");

          std::cout << name << " {id: " << cust_id << "; status: " << status
                    << "; query time non-prepared: "
                    << swt.ElapsedMilliseconds() << "ms;}" << std::endl;

          swt.Reset();
        }
      }

      std::cout << "\nQuery Execution Non Prepared Total time: "
                << sw.ElapsedMilliseconds() << "ms" << "\n"
                << std::endl;
      sw.Reset();

      for (size_t i = 0; i < 10; i++) {
        ExecutionResultPtr result =
            tx->Execute(query, {status_filter, user_id_filter});

        if (result->Empty()) {
          std::cout << "No customers data.." << std::endl;
          server->Shutdown();
          return 0;
        }

        auto cursor = Cursor(*result);
        for (const auto row : cursor) {
          auto dyn = Mapper::Dynamic<int32_t, std::string, int16_t>(
              row, {"user_id", "username", "status"});

          auto cust_id = std::get<0>(dyn);  // row->As<int32_t>("user_id");
          auto name = std::get<1>(dyn);     // row->As<std::string>("username");
          auto status = std::get<2>(dyn);   // row->As<int16_t>("status");

          std::cout << name << " {id: " << cust_id << "; status: " << status
                    << "; query time prepared: " << swt.ElapsedMilliseconds()
                    << "ms;}" << std::endl;

          swt.Reset();
        }
      }

      std::cout << "\nQuery Execution Total time: " << sw.ElapsedMilliseconds()
                << "ms\n" << std::endl;
    }

    // simulate server running
    // testing the ping & cleaner interval tasks
    std::this_thread::sleep_for(std::chrono::seconds(60));
    std::cout << std::endl;
    
    server->Shutdown();
  } catch (const StorageException& e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}