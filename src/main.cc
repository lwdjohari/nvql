#include <iostream>
#include <memory>

#include "nvserv/storages/postgres/pg_server.h"

int main() {
  using namespace nvserv::storages;
  using namespace nvserv::storages::parameters;

  ParameterArgs params = {
    Param::SmallInt(16), 
    Param::String("Hey world"),
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

  // Connect to standalone Postgress DB Server
  // Connection pool with 5 connections standby, max 10 connections
  // auto clusters = {postgres::PgClusterConfig(
  //     "db-example", "the-user", "the-password", "localhost", 5433)};
  // StorageServerPtr server =
  //     std::make_shared<postgres::PgServer>(clusters, 5, 10);

  // try {
  //   server->TryConnect();
  //   auto tx = server->Begin();

  //   int32_t customer_status = 1;

  //   auto result = tx->Execute(
  //       "select * from customer where status = $1", customer_status);

  //   if (result->Empty()) {
  //     std::cout << "No customers data.." << std::endl;
  //     server->Shutdown();
  //     return 0;
  //   }

  //   for (const auto row : *result) {
  //     auto cust_id = row->As<int32_t>("cust_id");
  //     auto name = row->As<std::string>("name");
  //     auto member_type = row->As<std::string>("member_type");
  //     auto phone_number = row->As<std::string>("phone_number");
  //     auto status = row->As<int32_t>("status");
  //     auto dob = row->AsDateTimeOffset<nvm::dates::DateTime>("dob");

  //     std::cout << "[" << cust_id << "] " << name << " (" << member_type <<
  //     ":"
  //               << phone_number << ", " << dob << ", " << status << ")"
  //               << std::endl;
  //   }

  //   server->Shutdown();

  // } catch (const StorageException& e) {
  //   std::cerr << e.what() << '\n';
  // }

  return 0;
}