//
// OpenGLUtil.h
//
// Library: GPU
// Package: GL
// Module:  OpenGL
// 

#pragma once

#include <cstdint>
#include <string>

#include "OpenGLCommon.h"

namespace Mmp
{

/**
 * @brief 查询 OpenGL Extension 相关信息
 * @note  1 - 只能调用一次
 *        2 - 需在 context  线程调用
 * @sa    openGLFeature
 */
bool CheckOpenGLExtensions();

/**
 * @brief 获取 OpenGL Extension 相关信息
 * @note  需在调用 CheckOpenGLExtensions 成功后才可调用
 */
const OpenGLFeature& GetOpenGLFeature();

/**
 * @brief 比较 OpenGL 版本号
 */
bool OpenGLVersionGreater(const OpenGLFeature& openGLFeature, int major, int minor);

/**
 * @note for debug
 */
void InfoOpenGLFeature(const OpenGLFeature& openGLFeature);

/**
 * @brief  清空所有 OpenGL 错误
 * @note   在渲染线程进入时调用
 */
void ClearError();

/**
 * @brief      检查 OpenGL 是否执行成功
 * @param[in]  file : 文件名
 * @param[in]  line : 所在的代码行
 */
bool CheckGLError(const std::string& file, uint64_t line);

/**
 * @brief     忽略 OpenGL 报错 
 */
void IgnoreGLError();

/**
 * @param[out]   f
 * @param[in]    u
 */
void Uint8x4ToFloat(float f[4], uint32_t u);

#ifdef ENABLE_OPENGL_DEBUG
#define CHECK_GL_ERROR_IF_DEBUG() assert(CheckGLError(std::string(__FILE__), __LINE__)) 
#define IGNORE_GL_ERROR_IF_DEBUG() IgnoreGLError()
#else
#define CHECK_GL_ERROR_IF_DEBUG()
#define IGNORE_GL_ERROR_IF_DEBUG()
#endif

} // namespace Mmp