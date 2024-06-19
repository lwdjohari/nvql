#pragma once

#include <chrono>
#include <ostream>

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 0
#include "nvserv/components/component.h"
#endif

#include "nvserv/global_macro.h"
#include "nvserv/storages/cluster_config.h"
#include "nvserv/storages/connection_pool.h"
#include "nvserv/storages/declare.h"

NVSERV_BEGIN_NAMESPACE(storages)

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 0
class StorageServer : public components::Component {
 public:
  StorageServer(const components::ComponentLocator& locator,
                const components::ComponentConfig& config,
                components::ComponentType type)
                  : components::Component(locator, config, type) {}
#endif

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
  class StorageServer {
   public:
    StorageServer() {}
#endif

    virtual const std::string& Name() const = 0;
      
    virtual ~StorageServer(){};

    virtual const StorageConfig& Configs() const = 0;

    virtual bool TryConnect() = 0;

    virtual bool Shutdown(
        bool grace_shutdown = true,
        std::chrono::seconds deadline = std::chrono::seconds(0)) = 0;

    virtual TransactionPtr Begin(
        TransactionMode mode = TransactionMode::ReadWrite) = 0;

    virtual const ConnectionPoolPtr Pool() const = 0;

    virtual ConnectionPoolPtr Pool() = 0;

    virtual StorageInfo GetStorageServerInfo() const = 0;
    
  };

  NVSERV_END_NAMESPACE