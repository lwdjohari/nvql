#pragma once

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1

#include <nvm/exceptions/exception.h>

#include "nvserv/global_macro.h"

NVSERV_BEGIN_ROOT_NAMESPACE

using Exception = nvm::Exception;
using BadAllocationException = nvm::BadAllocationException;
using CastException = nvm::CastException;
using InvalidArgException = nvm::InvalidArgException;
using NullReferenceException = nvm::NullReferenceException;
using OutOfBoundException = nvm::OutOfBoundException;
using RuntimeException = nvm::RuntimeException;
using ThreadException = nvm::ThreadException;
using TypeException = nvm::TypeException;

NVSERV_END_NAMESPACE

#endif