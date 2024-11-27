#include "nvserv/storages/cluster_config.h"

// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

ClusterConfig::ClusterConfig(StorageType type, const std::string& user,
                             const std::string& password,
                             const std::string& host, const uint32_t port)
                : user_(std::string(user)),
                  password_(std::string(password)),
                  host_(host),
                  port_(port),
                  type_(type) {}

ClusterConfig::~ClusterConfig() {}

const std::string& ClusterConfig::User() const {
  return user_;
}

const std::string& ClusterConfig::Password() const {
  return password_;
}

const std::string& ClusterConfig::Host() const {
  return host_;
}

uint32_t ClusterConfig::Port() const {
  return port_;
}

StorageType ClusterConfig::Type() const {
  return type_;
}

std::string ClusterConfig::GetConfig() const {
  throw std::runtime_error(
      "Implement ClusterConfig::GetConfig on derrived class");
}

NVSERV_END_NAMESPACE