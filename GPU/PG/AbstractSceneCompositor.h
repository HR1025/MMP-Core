//
// AbstractSceneCompositor.h
//
// Library: GPU
// Package: Program
// Module:  Scene Compositor
// 

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "GPU/GL/GLContex.h"

#include "SceneCommon.h"
#include "AbstractSceneItem.h"
#include "AbstractSceneLayer.h"

namespace Mmp
{
namespace Gpu 
{

class AbstractSceneCompositor
{
public:
    using ptr = std::shared_ptr<AbstractSceneCompositor>;
public:
    AbstractSceneCompositor() = default;
    virtual ~AbstractSceneCompositor() = default;
public:
    static AbstractSceneCompositor::ptr Create();
public:
    /**
     * @brief    设置相关参数
     */
    virtual bool SetParam(const SceneCompositorParam& param) = 0;
    /**
     * @brief    获取相关参数
     */
    virtual SceneCompositorParam GetParam() = 0;
    /**
     * @brief       添加图层
     * @param[in]   tag : 唯一标识
     * @param[in]   layer
     */
    virtual bool AddSceneLayer(const std::string& tag, AbstractSceneLayer::ptr layer) = 0;
    /**
     * @brief      移除图层
     * @param[in]  tag : 唯一标识
     */
    virtual void DelSceneLayer(const std::string& tag) = 0;
    /**
     * @brief      获取图层
     * @param[in]  tag : 唯一标识
     */
    virtual AbstractSceneLayer::ptr GetSceneLayer(const std::string& tag) = 0;
    /**
     * @brief     获取所有图层标识
     */
    virtual std::vector<std::string /* tag */> GetSceneLayersTag() = 0;
    /**
     * @brief  绘画
     * @note   完成所有图层的合成
     */
    virtual void Draw() = 0;
    /**
     * @brief  获取绘制缓存区
     */
    virtual Texture::ptr GetFrameBuffer() = 0;
};

} // namespace Gpu 
} // namespace Mmp