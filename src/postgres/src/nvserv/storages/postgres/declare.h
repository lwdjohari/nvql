#pragma once
#include <memory>
#include "nvserv/global_macro.h"

NVSERV_BEGIN_NAMESPACE(storages::postgres)

class PgServer;
class PgTransaction;

using PgServerPtr = std::shared_ptr<PgServer>;
using PgTransactionPtr = std::shared_ptr<PgTransaction>;

NVSERV_END_NAMESPACE