#include <ostream>
#include <pqxx/pqxx>

#include "nvserv/global_macro.h"
#include "nvserv/storages/cluster_config.h"
#include "nvserv/storages/config.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgClusterConfig final : public storages::ClusterConfig {
 private:
//   std::vector<std::string> cluster_conninfo = {
//       "dbname=test user=postgres password=secret hostaddr=192.168.1.1 "
//       "port=5432",
//       "dbname=test user=postgres password=secret hostaddr=192.168.1.2 "
//       "port=5432",
//       "dbname=test user=postgres password=secret hostaddr=192.168.1.3 "
//       "port=5432"};

  std::string dbname_;
  

 public:
  explicit PgClusterConfig(const std::string& dbname, const std::string& user,
                          const std::string& password, const std::string& host,
                          uint32_t port)
                  : ClusterConfig(StorageType::Postgres, user, password, host,
                                  port),
                    dbname_(dbname) {};

  const __NR_STRING_COMPAT_REF DbName() const {
    return dbname_;
  }

  std::string GetConfig() const override {
    return __NR_RETURN_MOVE(std::ostringstream().str());
  }
};


NVSERV_END_NAMESPACE