//
// AbstractSceneLayer.h
//
// Library: GPU
// Package: Program
// Module:  Scene Layer
// 

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "GPU/GL/GLContex.h"

#include "SceneCommon.h"
#include "AbstractSceneItem.h"

namespace Mmp
{
namespace Gpu 
{

class AbstractSceneLayer
{
public:
    using ptr = std::shared_ptr<AbstractSceneLayer>;
public:
    AbstractSceneLayer() = default;
    virtual ~AbstractSceneLayer() = default;
public:
    static AbstractSceneLayer::ptr Create();
public:
    /**
     * @brief    设置相关参数
     */
    virtual bool SetParam(const SceneLayerParam& param) = 0;
    /**
     * @brief    获取相关参数
     */
    virtual SceneLayerParam GetParam() = 0;
    /**
     * @brief      添加元素
     * @param[in]  tag : 唯一标识
     * @param[in]  item
     */
    virtual bool AddSceneItem(const std::string& tag, AbstractSceneItem::ptr item) = 0;
    /**
     * @brief      删除元素
     * @param[in]  tag : 唯一标识
     */
    virtual void DelSceneItem(const std::string& tag) = 0;
    /**
     * @brief      获取元素
     * @param[in]  tag : 唯一标识
     */
    virtual AbstractSceneItem::ptr GetSceneItem(const std::string& tag) = 0;
    /**
     * @brief     获取所有元素标识
     */
    virtual std::vector<std::string /* tag */> GetSceneItemsTag() = 0;
    /**
     * @brief     更新画布
     * @param[in] canvas
     */
    virtual void UpdateCanvas(Texture::ptr canvas) = 0;
    /**
     * @brief    绘画
     * @param    framebuffer
     * @note     完成图层绘制
     */
    virtual void Draw(Texture::ptr framebuffer) = 0;
};

} // namespace Gpu 
} // namespace Mmp