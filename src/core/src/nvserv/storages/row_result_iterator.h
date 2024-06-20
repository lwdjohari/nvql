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

#pragma once

#include <iterator>
#include <memory>

#include "nvserv/global_macro.h"
#include "nvserv/storages/declare.h"

NVSERV_BEGIN_NAMESPACE(storages)

class RowResultIterator {
 public:
  virtual ~RowResultIterator();
  virtual RowResultIterator& operator++() = 0;
  virtual bool operator==(const RowResultIterator& other) const = 0;
  virtual bool operator!=(const RowResultIterator& other) const = 0;
  virtual RowResultPtr operator*() const = 0;
  virtual std::unique_ptr<RowResultIterator> clone() const = 0;
};

NVSERV_END_NAMESPACE
