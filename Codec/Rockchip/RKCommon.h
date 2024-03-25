//
// RKCommon.h
//
// Library: Common
// Package: Codec
// Module:  Rockchip
// 



#pragma once

#include "Codec/CodecCommon.h"
#include "Codec/AbstractEncoder.h"
#include "Codec/AbstractDecorder.h"

#include "rockchip/rk_type.h"
#include "rockchip/mpp_buffer.h"
#include "rockchip/rk_mpi.h"

#define RK_LOG_TRACE   MMP_MLOG_TRACE("RK")    
#define RK_LOG_DEBUG   MMP_MLOG_DEBUG("RK")    
#define RK_LOG_INFO    MMP_MLOG_INFO("RK")     
#define RK_LOG_WARN    MMP_MLOG_WARN("RK")     
#define RK_LOG_ERROR   MMP_MLOG_ERROR("RK")    
#define RK_LOG_FATAL   MMP_MLOG_FATAL("RK")   

#define RK_OP_SUCCESS(operation) ((rkRet = operation) == MPP_RET::MPP_OK)
#define RK_OP_FAIL(operation)    ((rkRet = operation) != MPP_RET::MPP_OK)

namespace Mmp
{
namespace Codec
{


} // namespace Codec
} // namespace Mmp