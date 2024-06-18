#include <iostream>
#include <memory>

#include "nvm/stopwatch.h"
#include "nvserv/storages/postgres/pg_server.h"

int main() {
  using namespace nvserv::storages;
  using namespace nvserv::storages::parameters;

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
  std::cout << "Param Size: " << sizeof(utc) << std::endl;

  // Only clusters, server declarations and dialect that are DB Specific
  // For NvQL executions are abstracted through unified API.

  // 'postgresql://mnvx:mnvx@172.21.240.1:5433/minechain'

  // Connect to standalone Postgress DB Server
  // Connection pool with 5 connections standby, max 10 connections
  auto clusters = {postgres::PgClusterConfig("minechain", "mnvx", "mnvx",
                                             "172.21.240.1", 5433)};
  StorageServerPtr server =
      std::make_shared<postgres::PgServer>(clusters, 1, 1);

  try {
    nvm::Stopwatch sw;

    server->TryConnect();
    std::cout << "Connect time: " << sw.ElapsedMilliseconds() << "ms"
              << std::endl;
    sw.Reset();

    std::cout << "Create Transact" << std::endl;
    auto tx = server->Begin();

    std::cout << "Transaction Creation: " << sw.ElapsedMilliseconds() << "ms"
              << std::endl;
    sw.Reset();

    int32_t customer_status = 1;
    nvm::Stopwatch swt;
    for (size_t i = 0; i < 10; i++) {
      ExecutionResultPtr result =
          tx->Execute("select * from users u inner join company c on "
                      "u.company_id = c.company_id");
      auto cursor = Cursor(*result);

      std::cout << "Query Trip time: " << swt.ElapsedMilliseconds() << "ms"
                << std::endl;
      swt.Reset();

      if (result->Empty()) {
        std::cout << "No customers data.." << std::endl;
        server->Shutdown();
        return 0;
      }

      // auto row =  result->At(0);

      // std::cout << "Row size: " << row->Size() << std::endl;
      for (const auto row : cursor) {
        auto cust_id = row->As<int32_t>("user_id");
        auto name = row->As<std::string>("username");
        auto status = row->As<int16_t>("status");

        std::cout << "[" << cust_id << "] " << name << " (" << status << ")"
                  << std::endl;
      }
    }

    std::cout << "Query Execution Total time: " << sw.ElapsedMilliseconds()
              << "ms" << std::endl;
    server->Shutdown();

  } catch (const StorageException& e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}