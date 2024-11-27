#include "nvserv/storages/cluster_config_list.h"

// cppcheck-suppress unknownMacro
NVSERV_BEGIN_NAMESPACE(storages)

ClusterConfigList::ClusterConfigList(StorageType type)
                : configs_(), type_(type) {}

const ClusterConfigListType& ClusterConfigList::Configs() const {
  return configs_;
}

ClusterConfigListType& ClusterConfigList::Configs() {
  return configs_;
}

size_t ClusterConfigList::Size() const {
  return configs_.size();
};

StorageType ClusterConfigList::Type() const {
  return type_;
}

NVSERV_END_NAMESPACE