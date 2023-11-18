//
// RPCCommon.h
//
// Library: RPC
// Package: RPC
// Module:  RPC
// 

#pragma once

#include <string>
#include <memory>
#include <functional>

#include "Common/Any.h"
#include "Common/LogMessage.h"
#include "Common/SharedData.h"

namespace Mmp
{
namespace RPC
{

#define RPC_LOG_TRACE   MMP_MLOG_TRACE("RPC")
#define RPC_LOG_DEBUG   MMP_MLOG_DEBUG("RPC")
#define RPC_LOG_INFO    MMP_MLOG_INFO("RPC")
#define RPC_LOG_WARN    MMP_MLOG_WARN("RPC")
#define RPC_LOG_ERROR   MMP_MLOG_ERROR("RPC")
#define RPC_LOG_FATAL   MMP_MLOG_FATAL("RPC")

using RPCDeserialize = std::function<bool(SharedData::ptr from, Any& to)>;
using RPCSerialize   = std::function<bool(const Any& form, SharedData::ptr& to)>;
using RPCProcess     = std::function<bool(const Any& request, Any& response)>;

} // namespace RPC
} // namespace Mmp