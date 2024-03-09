//
// SceneLayerImpl.h
//
// Library: GPU
// Package: Program
// Module:  Scene Layer
// 

#pragma once

#include <mutex>
#include <memory>
#include <string>
#include <unordered_map>

#include "AbstractSceneLayer.h"

namespace Mmp 
{
namespace Gpu 
{

class SceneLayerProgram;

class SceneLayerImpl : public AbstractSceneLayer
{
public:
    using ptr = std::shared_ptr<SceneLayerImpl>;
public:
    SceneLayerImpl();
public:
    bool SetParam(const SceneLayerParam& param)  override;
    SceneLayerParam GetParam() override;
    bool AddSceneItem(const std::string& tag, AbstractSceneItem::ptr item)  override;
    void DelSceneItem(const std::string& tag)  override;
    AbstractSceneItem::ptr GetSceneItem(const std::string& tag)  override;
    std::vector<std::string /* tag */> GetSceneItemsTag()  override;
    void UpdateCanvas(Texture::ptr canvas) override;
    void Draw(Texture::ptr framebuffer)  override;
private:
    std::mutex _mtx;
    std::shared_ptr<SceneLayerProgram> _program;
    Texture::ptr _canvas;
    SceneLayerParam _param;
private:
    std::mutex _itemMtx;
    std::unordered_map<std::string /* tag */, AbstractSceneItem::ptr> _items;
};

} // namespace Gpu 
} // namespace Mmp