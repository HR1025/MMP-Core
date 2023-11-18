//
// X11Window.h
//
// Library: GPU
// Package: Windows
// Module:  X11
// 

#pragma once

#include "AbstractWindows.h"

#include <GL/glx.h>

namespace Mmp
{

/**
 * @note 基于 X11 实现
 */
class X11Window : public AbstractWindows
{
public:
    X11Window();
public:
    bool Open() override;
    bool Close() override;
    bool BindRenderThread(bool bind) override;
    bool SetAPIType(APIType type) override;
    bool SetRenderMode(bool onScreen) override;
    WindowInfo GetInfo() override;
    void  Swap() override;
private:
    Display*      _dispaly;
    GLXFBConfig*  _fbConfig;
    APIType       _apiType;
    bool          _onScreen;
    uint32_t      _majorVersion;
    uint32_t      _minorVersion;
    GLXPbuffer    _pbuffer;
    GLXWindow     _window;
    XVisualInfo*  _visualInfo;
    GLXContext    _context;
};

} // namespace Mmp
