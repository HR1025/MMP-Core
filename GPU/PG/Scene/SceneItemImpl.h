//
// SceneItemImpl.h
//
// Library: GPU
// Package: Program
// Module:  Scene Item
// 

#pragma once

#include <mutex>
#include <memory>

#include "AbstractSceneItem.h"

namespace Mmp 
{
namespace Gpu 
{

class SceneItemProgram;

class SceneItemImpl : public AbstractSceneItem
{
public:
    SceneItemImpl();
public:
    bool SetParam(const SceneItemParam& params) override;
    SceneItemParam GetParam() override;
    void UpdateImage(Texture::ptr image) override;
    void Draw(Texture::ptr canvas) override;
private:
    std::mutex _mtx;
    std::shared_ptr<SceneItemProgram> _program;
    Texture::ptr _image;
    SceneItemParam _params;
};

} // namespace Gpu 
} // namespace Mmp