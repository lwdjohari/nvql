#include <iostream>
#include <memory>

#include "nvserv/storages/postgres/pg_server.h"

int main() {
  using namespace nvserv::storages;

  // Only clusters, server declarations and dialect that are DB Specific
  // For NvQL executions are abstracted through unified API.
   
  // Connect to standalone Postgress DB Server
  // Connection pool with 5 connections standby, max 10 connections
  auto clusters = {postgres::PgClusterConfig(
      "db-example", "the-user", "the-password", "localhost", 5433)};
  StorageServerPtr server = std::make_shared<postgres::PgServer>(clusters, 5, 10);

  try {
    server->TryConnect();
    auto tx = server->Begin();

    int32_t customer_status = 1;
    auto result = tx->Execute("select * from customer where status = $1",
                              customer_status);

    if (result->Empty()) {
      std::cout << "No customers data.." << std::endl;
      server->Shutdown();
      return 0;
    }

    for (const auto row : *result) {
      auto cust_id = row->As<int32_t>("cust_id");
      auto name = row->As<std::string>("name");
      auto member_type = row->As<std::string>("member_type");
      auto phone_number = row->As<std::string>("phone_number");
      auto status = row->As<int32_t>("status");
      auto dob = row->AsDateTimeOffset<nvm::dates::DateTime>("dob");

      std::cout << "[" << cust_id << "] " << name << " (" << member_type << ":"
                << phone_number << ", " << dob << ", " << status << ")"
                << std::endl;
    }

    server->Shutdown();

  } catch (const StorageException& e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}