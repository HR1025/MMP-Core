//
// SceneCompositorImpl.h
//
// Library: GPU
// Package: Program
// Module:  Scene Compositor
// 

#pragma once

#include <atomic>
#include <vector>
#include <mutex>
#include <string>
#include <unordered_map>

#include "AbstractSceneCompositor.h"

#include "GPU/GL/GLContex.h"
#include "GPU/GL/GLDrawContex.h"

namespace Mmp
{
namespace Gpu 
{

class SceneCompositorImpl : public AbstractSceneCompositor
{
public:
    SceneCompositorImpl();
    ~SceneCompositorImpl();
public:
    bool SetParam(const SceneCompositorParam& param) override;
    SceneCompositorParam GetParam() override;
    bool AddSceneLayer(const std::string& tag, AbstractSceneLayer::ptr layer) override;
    void DelSceneLayer(const std::string& tag) override;
    AbstractSceneLayer::ptr GetSceneLayer(const std::string& tag) override;
    std::vector<std::string /* tag */> GetSceneLayersTag() override;
    void Draw() override;
    Texture::ptr GetFrameBuffer() override;
private:
    std::mutex _mtx;
    GLDrawContex::ptr _draw;
    SceneCompositorParam _param;
private:
    std::mutex _layerMtx;
    std::unordered_map<std::string /* tag */, AbstractSceneLayer::ptr> _layers;
private:
    std::mutex _fbMtx;
    std::vector<Texture::ptr> _frameBuffers;
    size_t _curFbIndex;
private:
    std::atomic<bool> _isInit;
};

} // namespace Gpu 
} // namespace Mmp