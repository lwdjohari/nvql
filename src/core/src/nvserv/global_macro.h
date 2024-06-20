/*
 * Copyright (c) 2024 Linggawasistha Djohari
 * <linggawasistha.djohari@outlook.com>
 * Licensed to Linggawasistha Djohari under one or more contributor license
 * agreements.
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership.
 *
 *  Linggawasistha Djohari licenses this file to you under the Apache License,
 *  Version 2.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#if defined(NVQL_STANDALONE) && NVQL_STANDALONE == 1
#pragma once



#if __cplusplus >= 201703L
#define __NR_CPP17 1
#else
#define __NR_CPP14 1
#endif

#if __NR_CPP17
#define __NR_RETURN_MOVE(arg) arg
#else
#define __NR_RETURN_MOVE(arg) std::move(arg)
#endif

#if __NR_CPP17
#include <string_view>
#endif

#if __NR_CPP17
#define __NR_STRING_COMPAT std::string_view
#else
#define __NR_STRING_COMPAT std::string
#endif

#if __NR_CPP17
#define __NR_STRING_COMPAT_REF std::string_view
#else
#define __NR_STRING_COMPAT_REF std::string&
#endif

#if __NR_CPP17
#define __NR_CONSTEXPR constexpr
#else
#define __NR_CONSTEXPR
#endif


#if __NR_CPP17
#define __NR_HANDLE_STRING_VIEW(view_name) \
reinterpret_cast<const std::string*>(view_name.data())
#else
#define __NR_HANDLE_STRING_VIEW(var_name,view_name) \
view_name
#endif

#if __NR_CPP17
#define __NR_CALL_STRING_COMPAT_REF(var) \
var.data()
#else
#define __NR_CALL_STRING_COMPAT_REF(var) \
var
#endif

#define NVSERV_NAMESPACE namespace nvserv
#define NVSERV_BEGIN_ROOT_NAMESPACE NVSERV_NAMESPACE {
#define NVSERV_BEGIN_NAMESPACE(arg) NVSERV_NAMESPACE::arg {
#define NVSERV_END_NAMESPACE }

#include "nvm/dates/datetime.h"

NVSERV_BEGIN_ROOT_NAMESPACE

using NvDateTime = nvm::dates::DateTime;

NVSERV_END_NAMESPACE
#endif