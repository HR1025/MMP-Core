#include "SceneCompositorImpl.h"

#include <cstdint>
#include <algorithm>
#include <mutex>
#include <vector>

#include "Common/PixelFormat.h"
#include "Common/PixelFormat.h"
#include "Utility/CommonUtility.h"

namespace Mmp
{
namespace Gpu 
{

SceneCompositorImpl::SceneCompositorImpl()
{
    _draw = GLDrawContex::Instance();
    _isInit = false;
    _curFbIndex = 0;
}

SceneCompositorImpl::~SceneCompositorImpl()
{

}

bool SceneCompositorImpl::SetParam(const SceneCompositorParam& param)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_isInit)
    {
        return false;
    }
    else
    {
        _param = param;
        return true;
    }
}

SceneCompositorParam SceneCompositorImpl::GetParam()
{
    std::lock_guard<std::mutex> paramLock(_mtx);
    return _param;
}

bool SceneCompositorImpl::AddSceneLayer(const std::string& tag, AbstractSceneLayer::ptr layer)
{
    std::lock_guard<std::mutex> lock(_layerMtx);
    bool foundLevelExit = false;
    {
        SceneLayerParam layerParam = layer->GetParam();
        for (const auto& val : _layers)
        {
            AbstractSceneLayer::ptr __layer = val.second;
            if (__layer->GetParam().level == layerParam.level)
            {
                foundLevelExit = true;
                break;
            }
        }
    }
    if (foundLevelExit)
    {
        return false;
    }
    else
    {
        SceneCompositorParam param = _param;
        _layers[tag] = layer;
        layer->UpdateCanvas(Gpu::Create2DTextures(_draw, PixelsInfo(param.width, param.height, 8, PixelFormat::RGBA8888), "Framebuffer", true)[0]);
        return true;
    }
}

void SceneCompositorImpl::DelSceneLayer(const std::string& tag)
{
    std::lock_guard<std::mutex> lock(_layerMtx);
    if (_layers.count(tag))
    {
        _layers[tag]->UpdateCanvas(nullptr);
    }
    _layers.erase(tag);
}

AbstractSceneLayer::ptr SceneCompositorImpl::GetSceneLayer(const std::string& tag)
{
    std::lock_guard<std::mutex> lock(_layerMtx);
    if (_layers.count(tag))
    {
        return _layers[tag];
    }
    else 
    {
        return nullptr;
    }
}

std::vector<std::string /* tag */> SceneCompositorImpl::GetSceneLayersTag()
{
    std::lock_guard<std::mutex> lock(_layerMtx);
    std::vector<std::string /* tag */> tags;
    for (const auto& val : _layers)
    {
        tags.push_back(val.first /* tag : string */);
    }
    return tags;
}

void SceneCompositorImpl::Draw()
{
    // 创建 FrameBuffer 缓冲区
    if (!_isInit)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        SceneCompositorParam param = _param;
        for (uint32_t i=0; i<_param.bufSize; i++)
        {
            _frameBuffers.push_back(Gpu::Create2DTextures(_draw, PixelsInfo(param.width, param.height, 8, PixelFormat::RGBA8888), "Framebuffer", true)[0]);
        }
        _isInit = true;
    }
    Texture::ptr frameBuffer = nullptr;
    size_t curFbIndex;
    // 获取 FrameBuffer 缓冲区
    {
        std::lock_guard<std::mutex> fbLock(_fbMtx);
        curFbIndex = (_curFbIndex + 1) % _frameBuffers.size();
        frameBuffer = _frameBuffers[curFbIndex];
    }

    std::lock_guard<std::mutex> lock(_mtx);
    std::vector<AbstractSceneLayer::ptr> laysers;
    {
        std::lock_guard<std::mutex> layerLock(_layerMtx);
        for (const auto& val : _layers)
        {
            laysers.push_back(val.second /* layer : AbstractSceneLayer::ptr */);
        }
    }
    std::sort(laysers.begin(), laysers.end(), [](AbstractSceneLayer::ptr left, AbstractSceneLayer::ptr right) -> bool
    {
        return left->GetParam().level < right->GetParam().level;
    });
    _draw->FenceBegin();
    {
        FrameBuffer::ptr fbo;
        {
            FrameBufferDesc desc;
            desc.width  = frameBuffer->Width();
            desc.height = frameBuffer->Height();
            desc.depth  = 0;
            desc.multiSampleLevel = 0;
            desc.numLayers = 1;
            desc.zStencil = 0;
            desc.colorTexs.push_back(frameBuffer);
            desc.tag = "SceneCompositorFrameBuffer"; 
            fbo = _draw->CreateFrameBuffer(desc);
        }
        {
            RenderPassInfo info;
            info.color      = RPAction::CLEAR;
            info.depth      = RPAction::CLEAR;
            info.stencil    = RPAction::CLEAR; 
            info.clearColor = 0x00000000;
            _draw->BindFramebufferAsRenderTarget(fbo, info);
        }
        _draw->FenceCommit();
    }
    for (auto layer : laysers)
    {
        layer->Draw(frameBuffer);
    }
    _draw->FenceEnd();
    // Update index
    {
        std::lock_guard<std::mutex> fbLock(_fbMtx);
        _curFbIndex = curFbIndex;
    }
}

Texture::ptr SceneCompositorImpl::GetFrameBuffer()
{
    std::lock_guard<std::mutex> fbLock(_fbMtx);
    return _frameBuffers[_curFbIndex];
}

} // namespace Gpu 
} // namespace Mmp