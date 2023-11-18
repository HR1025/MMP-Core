//
// SDLWindow.h
//
// Library: GPU
// Package: Windows
// Module:  SDL
// 

#pragma once

#include "AbstractWindows.h"

struct SDL_Window;

namespace Mmp
{

/**
 * @note 基于 SDL 实现
 */
class SDLWindow : public AbstractWindows
{
public:
    SDLWindow();
public:
    bool Open() override;
    bool Close() override;
    bool BindRenderThread(bool bind) override;
    bool SetAPIType(APIType type) override;
    bool SetRenderMode(bool onScreen) override;
    WindowInfo GetInfo() override;
    void  Swap() override;
private:
    bool InitSDL();
    void UnInitSDL();
private:
    int            _mode;
    SDL_Window*    _window;
    void*          _contex;
    APIType        _type;
    uint32_t       _major;
    uint32_t       _minor;
private:
    uint32_t       _displayWidth;
    uint32_t       _displayHeight;
    uint32_t       _displayRefresh;
    std::string    _title;
    bool           _isInited;
};

} // namespace Mmp
