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

#include <nvm/macro.h>

#include "nvserv/global_macro.h"
#include "ostream"

NVSERV_BEGIN_ROOT_NAMESPACE

enum class ServerType
{
  Http2Auto,
  Http2,
  Http1,
  Grpc,
  TcpServer,
  UdpServer
};
NVM_ENUM_CLASS_DISPLAY_TRAIT(ServerType)

NVM_ENUM_TO_STRING_FORMATTER(ServerType, case ServerType::Http2Auto
                             // cppcheck-suppress syntaxError
                             : return "Htpp2Auto";
                             case ServerType::Http2
                             : return "Http2";
                             case ServerType::Http1
                             : return "Http1";
                             case ServerType::Grpc
                             : return "Grpc";
                             case ServerType::TcpServer
                             : return "TcpServer";
                             case ServerType::UdpServer
                             : return "UdpServer";)

enum class TaskPoolMode
{
  None = 0,
  Internal = 1,
  SharedComponent = 2,
  SharedGlobal = 3
};

NVM_ENUM_CLASS_DISPLAY_TRAIT(TaskPoolMode)

NVM_ENUM_TO_STRING_FORMATTER(TaskPoolMode, case TaskPoolMode::None
                             : return "None";
                             case TaskPoolMode::Internal
                             : return "Internal";
                             case TaskPoolMode::SharedComponent
                             : return "SharedComponent";
                             case TaskPoolMode::SharedGlobal
                             : return "SharedGlobal";)

NVSERV_END_NAMESPACE

#endif