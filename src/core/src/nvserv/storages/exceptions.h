#pragma once

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"
#include "nvserv/exceptions.h"

NVSERV_BEGIN_NAMESPACE(storages)

class StorageException : public Exception {
 public:
  explicit StorageException(const std::string& message, const StorageType& type)
                  : Exception(message), type_(StorageType(type)) {}

  const StorageType& Type() const {
    return type_;
  }

 private:
  StorageType type_;
};

class ConnectionException : public StorageException {
 public:
  explicit ConnectionException(const std::string& message,
                               const StorageType& type)
                  : StorageException(message, type) {}
};

class TransactionException : StorageException {
 public:
  explicit TransactionException(const std::string& message,
                                const StorageType& type)
                  : StorageException(message, type) {}
};

class ExecutionException : TransactionException {
 public:
  explicit ExecutionException(const std::string& message,
                              const StorageType& type)
                  : TransactionException(message, type) {}
};

class InternalErrorException : StorageException {
 public:
  explicit InternalErrorException(const std::string& message,
                                  const StorageType& type)
                  : StorageException(message, type) {}
};

class ParameterTypeException : StorageException {
 public:
  explicit ParameterTypeException(const std::string& message,
                                  const StorageType& type)
                  : StorageException(message, type) {}
};

class UnsupportedFeatureException : StorageException {
 public:
  explicit UnsupportedFeatureException(const std::string& message,
                                       const StorageType& type)
                  : StorageException(message, type) {}
};

NVSERV_END_NAMESPACE