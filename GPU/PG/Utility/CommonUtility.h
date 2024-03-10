//
// CommonUtility.h
//
// Library: GPU
// Package: Program
// Module:  Common
// 

#pragma once

#include <vector>
#include <string>
#include "Common/PixelFormat.h"
#include "Common/PixelsInfo.h"
#include "Common/AbstractFrame.h"
#include "Common/AbstractPicture.h"
#include "GPU/GL/GLDrawContex.h"

namespace Mmp
{
namespace Gpu
{

/**
 * @brief       创建2D纹理
 * @param[in]   draw         : 绘制句柄
 * @param[in]   info         : 像素描述信息
 * @param[in]   tag          : 纹理标识符
 * @param[in]   useForRender : 是否用于渲染
 */
std::vector<Texture::ptr> Create2DTextures(GLDrawContex::ptr draw, const PixelsInfo& info, const std::string& tag = std::string(), bool useForRender = false);

/**
 * @brief       更新2D纹理
 * @param[in]   draw  : 绘制句柄
 * @param[in]   texs  : 需要更新的纹理
 * @param[in]   frame : 数据帧 
 */
void Update2DTextures(GLDrawContex::ptr draw, std::vector<Texture::ptr> texs, AbstractPicture::ptr frame);

/**
 * @brief        拷贝2D纹理到内存中
 * @param[in]    draw  : 绘制句柄
 * @param[in]    texs  : 需要拷贝的纹理
 * @param[out]   frame : 数据帧
 * @return       是否读取成功 
 */
bool Copy2DTexturesToMemory(GLDrawContex::ptr draw, std::vector<Texture::ptr> texs, AbstractPicture::ptr frame);

} // namespace Gpu
} // namespace Mmp