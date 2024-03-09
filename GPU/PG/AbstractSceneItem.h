//
// AbstractSceneItem.h
//
// Library: GPU
// Package: Program
// Module:  Scene Item
// 

#pragma once

#include <memory>

#include "GPU/GL/GLContex.h"
#include "SceneCommon.h"

namespace Mmp 
{
namespace Gpu 
{

/**
 * @brief 场景元素
 */
class AbstractSceneItem
{
public:
    using ptr = std::shared_ptr<AbstractSceneItem>;
public:
    AbstractSceneItem() = default;
    virtual ~AbstractSceneItem() = default;
public:
    static AbstractSceneItem::ptr Create();
public:
    /**
     * @brief    设置相关参数配置
     */
    virtual bool SetParam(const SceneItemParam& params) = 0;
    /**
     * @brief    获取相关配置
     */
    virtual SceneItemParam GetParam() = 0;
    /**
     * @brief 更新元素
     */
    virtual void UpdateImage(Texture::ptr image) = 0;
    /**
     * @brief      绘画
     * @param[in]  canvas
     * @note       将元素内容绘制到画布上
     */
    virtual void Draw(Texture::ptr canvas) = 0;
};

} // namespace Gpu 
} // namespace Mmp