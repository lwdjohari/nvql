#pragma once

#include <nvm/types/type_utility.h>

#include <ostream>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"

NVSERV_BEGIN_NAMESPACE(storages)

class ClusterConfigBase {
 public:
  virtual StorageType Type() const = 0;
  virtual const std::string& User() const = 0;
  virtual const std::string& Password() const = 0;
  virtual const std::string& Host() const = 0;
  virtual std::string GetConfig() const = 0;
  virtual uint32_t Port() const = 0;
};

class ClusterConfigList {
 public:
  explicit ClusterConfigList(StorageType type) : configs_(), type_() {}

  template <typename T>
  void Add(T&& config) {
    using namespace nvm::types::utility;

    static_assert(is_has_base_of_v<T, ClusterConfig>,
                  "Type must be derrived type from \"ClusterConfig\"");

    if (config.Type() != type_)
      throw std::invalid_argument(
          "ClusterConfigList is set to only accept StorageType = " +
          ToStringEnumStorageType(type_));

    configs_.push_back(std::forward<T>(config));
  };

  const ClusterConfigListType& Configs() const {
    return configs_;
  }

  ClusterConfigListType& Configs() {
    return configs_;
  }

  size_t Size() const {
    return configs_.size();
  };

  StorageType Type() const {
    return type_;
  }

 protected:
  ClusterConfigListType configs_;
  StorageType type_;
};

class ClusterConfig : public ClusterConfigBase {
 public:
  virtual ~ClusterConfig() {}

 protected:
  explicit ClusterConfig(StorageType type, const std::string& user,
                         const std::string& password, const std::string& host,
                         const uint32_t port)
                  : user_(std::string(user)),
                    password_(std::string(password)),
                    host_(host),
                    port_(port),
                    type_(type) {}

  const std::string& User() const override {
    return user_;
  }

  const std::string& Password() const override {
    return password_;
  }

  const std::string& Host() const override {
    return host_;
  }

  uint32_t Port() const override {
    return port_;
  }

  StorageType Type() const override {
    return type_;
  }

  virtual std::string GetConfig() const override {
    throw std::runtime_error(
        "Implement ClusterConfig::GetConfig on derrived class");
  }

 private:
  std::string user_;
  std::string password_;
  std::string host_;
  uint32_t port_;
  StorageType type_;
};

NVSERV_END_NAMESPACE